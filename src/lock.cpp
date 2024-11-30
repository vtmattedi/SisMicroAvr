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
            return;
        }
    }
    // If we reach here, the password was not found
    protoStringAssign(&password, "");
    if (this->locked)
    {
        this->guessCount++;
        serialPrintf("Incorrect password remmaing attemps: %d\n", 3 - this->guessCount);
        if (this->guessCount >= 3)
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
    protoStringAssign(&password, "");
    this->locked = true;
    this->guessCount = 0;
    pinWrite(this->relayPin, LOW);
    this->lastOpen = 0;
    this->stateChaged = true;
}

void Lock::HandleInput(int key)
{
    unsigned long now = millis();
    if (key == KEYPAD_NONE)
    {
        if ((now - this->lastOpen > 5000 && !this->locked) || (now - this->lastInput > TIME_TO_STALE))
        {
            this->lastInput = now;
            this->reset();
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
            this->tryUnlock();
            this->stateChaged = true;
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
        if (data[0] == 0xFF)
        {
            break;
        }
        lockHandler.numUsers++;
        addr += ADDR_MULTIPLIER;
    }
    serialPrintf(" %d users found in EEPROM!\n", lockHandler.numUsers);
}
