#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
// Our files just for organization
#include <eeprom.h>
#include <serialutil.h>
#include <pinutil.h>
#include <keyPad.h>

#include <lcd.h>

#include <timing.h>
//Should be in a separate file and now it is :)
#include <protostring.h>

ProtoString passwd = ProtoString(); 


void cnp()
{
    clear_lcd();
    lcd_set_cursor(0, 0);
    print(passwd.data);
    lcd_set_cursor(0, 15);
    print("*");

    

}

int main(void)
{
    // Setup the keypad
    setupKeyPad();
    // Setup the serial port
    serialBegin();
    lcd_init();
    millis_init();
    static int count = 0;
    static int lastKey = KEYPAD_NONE;
    set_seconds(1732886116);
    DateTime dt;
    while (true)
    {
        int key = readKeypad();
        if (key != lastKey)
        {
            lastKey = key;
            if (key != KEYPAD_NONE)
            {
                if (key == KEYPAD_STAR)
                {
                    // Clear the password
                    passwd = "";
                    cnp();
                }
                else
                {
                passwd += keyMap[key];
                cnp();
                }
                dt.Calculate();
                char dateBuffer[50];
                sprintf(dateBuffer, "Time: %d/%d/%d %d:%d:%d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
                serialPrint(dateBuffer);
                serialPrint("Passwd: \"");
                serialPrint(passwd.data);
                serialPrint("\"\n");
                char buffer [50];
                sprintf(buffer, "Key: %c\n", keyMap[key]);
                serialPrint(buffer);

            }
        }

    }

    return 1;
}

