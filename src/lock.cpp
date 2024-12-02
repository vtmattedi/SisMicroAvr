#include <lock.h>
// not gonna implement the inverted feature for now

Lock lockHandler;

void Lock::init(int relayPin, int alarmPin)
{
    this->relayPin = relayPin;
    this->alarmPin = alarmPin;
    pinMode(relayPin, OUTPUT);
    pinMode(alarmPin, OUTPUT);
    pinWrite(relayPin, LOW);
    pinWrite(alarmPin, LOW);
    this->locked = true;
    this->alarmRaised = false;
    this->guessCount = 0;
    protoStringAssign(&password, "");
    this->state = IDLE;
    this->loadUsers();
    this->stateChaged = false;
}

void Lock::tryUnlock()
{
    serialPrintTime();
    serialPrint("Unlock attempt \n");

    for (int i = 0; i < numUsers; i++)
    {
        uint16_t addr = i * ADDR_MULTIPLIER;
        uint8_t data[4];
        EEPROM_read_batch(addr, data, 4);
        if (data[0] == password.data[0] && data[1] == password.data[1] && data[2] == password.data[2] && data[3] == password.data[3])
        {
            serialPrintTime();
            serialPrintf("User %d unlocked the lock\n", i);
            this->locked = false;
            this->guessCount = 0;
            protoStringAssign(&password, "");
            pinWrite(this->relayPin, HIGH);
            this->currentUser = i;
            this->lastOpen = millis();
            this->endAlarm();
            this->hashtagCount = 0;
            return;
        }
    }
    // If we reach here, the password was not found
    protoStringAssign(&password, "");
    if (this->locked)
    {
        this->guessCount++;
        serialPrintf("Incorrect password remmaing attemps: %d\n", MAX_GUESSES - this->guessCount);
        if (this->guessCount >= MAX_GUESSES)
        {
            this->raiseAlarm();
        }
    }
    // else
    // {
    //     serialPrint("Lock is already open\n");
    // }
}

void Lock::raiseAlarm()
{
    if (this->alarmRaised)
    {
        return;
    }
    this->alarmRaised = true;
    pinWrite(this->alarmPin, HIGH);
    this->stateChaged = true;
}

void Lock::endAlarm()
{
    if (!this->alarmRaised)
    {
        return;
    }

    this->alarmRaised = false;
    pinWrite(this->alarmPin, LOW);
    this->stateChaged = true;
}

void Lock::reset()
{
    if (this->password.length != 0 || this->locked == false)
    {
        this->stateChaged = true;
    }
    protoStringAssign(&password, "");
    this->locked = true;
    this->guessCount = 0;
    pinWrite(this->relayPin, LOW);
    this->lastOpen = 0;
    protoStringAssign(&this->message, "");
}

void Lock::HandleInput(int key)
{
    unsigned long now = millis();
    if (key == KEYPAD_NONE)
    {
        if (this->state == IDLE)
        {
            if ((now - this->lastOpen > TIME_TO_LOCK && !this->locked) || (now - this->lastInput > TIME_TO_STALE))
            {
                this->reset();
            }
        }
        else if (this->state == REGISTERING)
        {
            if (now - this->lastInput > TIME_TO_STALE)
            {
                this->state = IDLE;
                this->reset();
            }
        }
        else if (this->state == MESSAGE)
        {
            if (now - this->lastOpen > TIME_TO_STALE)
            {
                this->state = IDLE;
                this->reset();
            }
        }
        return;
    }
    this->lastInput = now;
    // Star key clears the password
    if (key == KEYPAD_STAR)
    {
        protoStringAssign(&this->password, ""); // clear password
        this->stateChaged = true;
    }
    else
    {
        // Passwords are 4 characters long
        if (this->password.length < 4 && key < 10)
        {
            protoStringAppendChar(&this->password, keyMap[key]);
            this->stateChaged = true;
        }
        // If the password is 4 characters long, we check if the user is trying to unlock
        if (this->password.length == 4 && key == KEYPAD_HASH)
        {
            if (this->state == IDLE)
            {
                this->tryUnlock();
            }
            else if (this->state == REGISTERING)
            {
                if (this->numUsers < MAX_USERS)
                {
                    uint8_t data[4] = {this->password.data[0], this->password.data[1], this->password.data[2], this->password.data[3]};
                    for (int i = 0; i < 4; i++)
                    {
                        int addr = this->numUsers * ADDR_MULTIPLIER + i;
                        int res = EEPROM_write(addr, data[i]);
                        if (res != EEPROM_OK)
                        {
                            serialPrintf("Error writing to EEPROM: %d\n", res);
                        }
                    }
                    this->numUsers++;
                    serialPrintTime();
                    serialPrintf("User %d registered\n", this->numUsers - 1);
                    protoStringAssign(&this->password, "");
                }
                else
                {
                    serialPrint("Max users reached!\n");
                }
                this->state = IDLE;
                this->reset();

            }
            this->stateChaged = true;
        }

        else if (!this->locked && key == KEYPAD_HASH && this->password.length == 0)
        {
            this->lastOpen = now;
            this->hashtagCount++;
            if (this->hashtagCount == 3)
            {
                this->state = REGISTERING;
                this->stateChaged = true;
                this->hashtagCount = 0;
            }
            this->stateChaged = true;
            char buffer[6];
            sprintf(buffer, "#x%d", this->hashtagCount);
            protoStringAssign(&this->message, buffer);
            
        }
    }
}

void Lock::loadUsers()
{
    serialPrint("Loading users:");
    uint16_t addr = 0;
    while (addr < EEPROM_SIZE)
    {
        uint8_t data[4];
        EEPROM_read_batch(addr, data, 4);
        // If the first byte is 0xFF, we reached the end of valid data
        // we always write 4 bytes at a time, to addresses that are multiples of 5
        // the EEPROM should look like:
        //  n * 5  | n*5 + 1| n*5 + 2| n*5 + 3| n*5 + 4
        // [user0-0][user0-1][user0-2][user0-3][0xFF]
        // [user1-0][user1-1][user1-2][user1-3][0xFF]...
        // ...
        // [userN-0][userN-1][userN-2][userN-3][0xFF]
        // [0xFF]   [0xFF]   [0xFF]   [0xFF]   [0xFF]
        if (data[0] == 0xFF)
        {
            break;
        }
        this->numUsers++;
        addr += ADDR_MULTIPLIER;
    }
    if (this->numUsers == 0)
    {
        serialPrint(" No users found in EEPROM!\n");
        this->state = REGISTERING;
    }
    else
        serialPrintf(" %d users found in EEPROM!\n", lockHandler.numUsers);
}
