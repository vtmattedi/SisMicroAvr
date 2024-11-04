#include <avr/io.h>
#include "eeprom.h"

int main (void)
{
    // Set PORTB as output
    DDRB = 0xFF;

    // Set PORTB low
    PORTB = 0x00;

    while (1)
    {
        // Set PORTB high
        PORTB = 0xFF;
    }

    return 1;
}