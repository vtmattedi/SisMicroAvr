#include <pinutil.h>
#include <keyPad.h>
#include <protostring.h>
#include <timing.h>
const char* stateMap[] = {"X","O"}
typedef enum LockState
{
    PASSWORD_INPUT,
    IDLE,
    LOCKED,
    UNLOCKED,
    REGISTER_PWD,
    ALARM
};
struct lock
{
    int relayPin;
    int ledPin;
    unsigned long lastInputTime;
    LockState state;
    bool state;
    bool alarmed;
    ProtoString currentGuess;
    void handleKeyPress(int key);
    void init(int relayPin, int ledPin);
    void reset();

}LockHandler;
