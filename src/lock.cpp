#include <lock.h>
// not gonna implement the inverted feature for now
#define ADDR_MULTIPLIER 5
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
}

void Lock::tryUnlock()
{
    for (int i = 0; i < numUsers; i++)
    {
        uint16_t addr = i * ADDR_MULTIPLIER;
        uint8_t data[4];
        EEPROM_read_batch(addr, data, 4);
        if (data[0] == password.data[0] && data[1] == password.data[1] && data[2] == password.data[2] && data[3] == password.data[3])
        {
            this->locked = false;
            this->alarmRaised = false;
            this->guessCount = 0;
            protoStringAssign(&password, "");
            return;
        }
    }
    // If we reach here, the password was not found
    this->guessCount++;
    if (this->guessCount >= 3)
    {
        this->alarmRaised = true;
        pinWrite(this->alarmPin, HIGH);
    }
}

void Lock::reset()
{
    protoStringAssign(&password, "");
    this->locked = true;
    this->guessCount = 0;
    pinWrite(this->relayPin, LOW);
}

void Lock::HandleInput(int key)
{
    if (key == KEYPAD_STAR)
    {
        protoStringAssign(&this->password, ""); // clear password
    }
    else
    {
        if (this->password.length < 4 && key < 10)
        {
            protoStringAppendChar(&this->password, keyMap[key]);
        }
        if (this->password.length == 4 && key == KEYPAD_HASH)
        {
            this->tryUnlock();
        }
    }
}

void Lock::RegisterUser()
{
    uint16_t addr = this->numUsers * ADDR_MULTIPLIER;
    EEPROM_write(addr, this->password.data[0]);
    EEPROM_write(addr + 1, this->password.data[1]);
    EEPROM_write(addr + 2, this->password.data[2]);
    EEPROM_write(addr + 3, this->password.data[3]);
    ;
    numUsers++;
}

void LoadUsers()
{
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
}
