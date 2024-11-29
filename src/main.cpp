#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//Our files just for organization
#include <eeprom.h>
#include <serialutil.h>
#include <pinutil.h>
#include <keyPad.h>

int main (void)
{
    //Setup the keypad
    setupKeyPad();
    //Setup the serial port
    serialBegin();
    while (1)
    {
        char* hw = "Hello World\n";
        serialPrint(hw);
        _delay_ms(1000);
    }

    return 1;
}