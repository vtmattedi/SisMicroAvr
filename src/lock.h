#pragma once
#include <pinutil.h>
#include <keyPad.h>
#include <protostring.h>
#include <timing.h>
#include <eeprom.h>
#include <serialutil.h>
#define ADDR_MULTIPLIER 5
#define MAX_USERS 10
#define TIME_TO_LOCK 5000 //After this time (ms) the lock will lock itself
#define TIME_TO_STALE 15000 //After this time (ms) the lock will reset the guess count and the current password
#define MAX_GUESSES 3 //After this number of incorrect guesses the alarm will be raised
enum LockState
{
    IDLE,
    REGISTERING,
    MESSAGE,
    ALARM
};

struct Lock
{
    int numUsers;
    int relayPin;
    int alarmPin;
    int hashtagCount;
    ProtoString password;
    bool locked;
    bool alarmRaised;
    int guessCount;
    void tryUnlock();
    void reset();
    void init(int relayPin, int alarmPin);
    void HandleInput(int key);
    void raiseAlarm();
    void endAlarm();
    void loadUsers();
    unsigned long lastInput; // millis() of the last input
    unsigned long lastOpen; // millis() of the last time the lock was open
    int currentUser;
    bool stateChaged;
    LockState state;
    ProtoString message;
};

extern Lock lockHandler;
