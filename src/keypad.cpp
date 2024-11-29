#include "keypad.h"
int readKeypad()
{
    int res = KEYPAD_NONE;
    for (int i = 0; i < 4; i++)
    {
        pinWrite(2 + i ,HIGH);
        if (pinRead(COL1))
        {
            res = 1 +  i * 3;
        }
        if (pinRead(COL2))
        {
            res = 1 + i * 3 + 1;
        }
        if (pinRead(COL3))
        {
            res = 1 + i * 3 + 2;
        }
    }

    // res = 1 + row * 3 + col 

    // if on the last row map special keys
    if (res == 10)
    {
        return KEYPAD_STAR;
    }
    else if (res == 11)
    {
        return 0;
    }
    else if (res == 12)
    {
        return KEYPAD_HASH;
    }
    
    return res;
}


void setupKeyPad()
{
    //setup the keypad
    pinMode(ROW1,OUTPUT);
    pinMode(ROW2,OUTPUT);
    pinMode(ROW3,OUTPUT);
    pinMode(ROW4,OUTPUT);
    pinMode(COL1,INPUT);
    pinMode(COL2,INPUT);
    pinMode(COL3,INPUT);
}