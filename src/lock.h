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
struct Lock
{
    int numUsers;
    int relayPin;
    int alarmPin;
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
};

extern Lock lockHandler;
