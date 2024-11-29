#include <pinutil.h>

//pin maps for the keypad (arduino pins)
//They are properly mapped to the pins on the ATmega328P on the pinutil.cpp file
#define ROW1 10
#define ROW2 11
#define ROW3 12
#define ROW4 13
#define COL1 14
#define COL2 15
#define COL3 16

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