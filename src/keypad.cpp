#include "keypad.h"

int getRowPin(int row)
{
    switch (row)
    {
    case 0:
        return ROW1;
    case 1:
        return ROW2;
    case 2:
        return ROW3;
    case 3:
        return ROW4;
    default:
        return -1;
    }
}
int getColPin(int col)
{
    switch (col)
    {
    case 0:
        return COL1;
    case 1:
        return COL2;
    case 2:
        return COL3;
    default:
        return -1;
    }
}
int readKeypad()
{
    int res = KEYPAD_NONE;
    for (int i = 0; i < 4; i++)
    {
        pinWrite(getRowPin(i) ,HIGH);
        for (int j = 0; j < 3; j++)
        {
            if (pinRead(getColPin(j)))
            {
                res = 1 + i * 3 + j;
            }
        }
        pinWrite(getRowPin(i) ,LOW);
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

char keyMap[] = {
    '0',
    '1','2','3',
    '4','5','6',
    '7','8','9',
    'C','#'
};