#include <lock.h>
// not gonna implement the inverted feature for now
#define GET_VAL(val,inverted) (inverted ? !val : val)
void lock::init(int relayPin, int ledPin)
{
    this.ledPin = ledPin;
    this.relayPin = relayPin;
    pinMode(ledPin, OUTPUT);
    pinWrite(ledPin, LOW);
    pinMode(relayPin, OUTPUT);
    pinWrite(relayPin, LOW);
    this.state = IDLE;
    this.currentGuess = "";
}
void lock::guessPwd()
{
    if (currentGuess == "1234")
    {
        state = UNLOCKED;
        pinWrite(pin, GET_VAL(HIGH, this.inverted));
    }
    else
    {
        state = LOCKED;
        pinWrite(pin, GET_VAL(LOW, this.inverted));
    }
}
//Returns to Idle, resets currents guess and lock the door
void lock::reset()
{
    this.state = IDLE;
    this.currentGuess = "";
    pinWrite(this.relayPin, LOW);
    pinWrite(this.ledPin, LOW);
}
#define seconds_to_idle 10
#define seconds_to_alarm 30
void lock::handleKeyPress(int key)
{
    if (key = KEYPAD_NONE)
    {
        int limit = this.alarmed ? seconds_to_alarm : seconds_to_idle;
        if (seconds() - lastInputTime > limit)
        {
            reset();
        }
        return;
    }

    this.lastInputTime = seconds();
    if (state == PASSWORD_INPUT)
    {
        if (key == KEYPAD_STAR)
        {

        }
        else if (key == KEYPAD_HASH)
        {
            guessPwd();
        }
        else
        {
            currentGuess += key;
        }
    }
}