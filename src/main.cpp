#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Our files just for organization
#include <eeprom.h>
#include <serialutil.h>
#include <pinutil.h>
#include <keyPad.h>

int main(void)
{
    // Setup the keypad
    setupKeyPad();
    // Setup the serial port
    serialBegin();
    while (1)
    {
        // Read the keypad
        int key = readKeypad();
        static int last_key = KEYPAD_NONE;
        // If a key was pressed
        if (key != KEYPAD_NONE)
        {
            if (key == KEYPAD_STAR && last_key == KEYPAD_STAR)
            {
                serialPrint("Double Star\n");
            }
            else
            {
                // Print the key to the serial port
                UDR0 = keyMap[key];
                while (!(UCSR0A & (1 << UDRE0)));
            }

            _delay_ms(100); // Debounce
            last_key = key;
        }
    }

    return 1;
}