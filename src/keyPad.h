#include <pinutil.h>

//pin maps for the keypad (arduino pins)
//They are properly mapped to the pins on the ATmega328P on the pinutil.cpp file
#define ROW1 2
#define ROW2 3
#define ROW3 4
#define ROW4 5
#define COL1 6
#define COL2 7
#define COL3 8

//Keypad layout
// 1 2 3
// 4 5 6
// 7 8 9
// * 0 #
#define KEYPAD_STAR 0xA
#define KEYPAD_HASH 0xB
#define KEYPAD_NONE -1


int readKeypad();
void setupKeyPad();

extern char keyMap[];