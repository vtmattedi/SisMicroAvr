#pragma once
#include <pinutil.h>
#include <keyPad.h>
#include <protostring.h>
#include <timing.h>
#include <eeprom.h>

#define MAX_USERS 10
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
    void RegisterUser();
    unsigned long lastInput; // millis() of the last input
    unsigned long lastOpen; // millis() of the last time the lock was open
    int currentUser;
};

extern Lock lockHandler;
