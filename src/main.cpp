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



struct ProtoString
{
    #define MAX_LENGTH 100
    char data[MAX_LENGTH];
    int length;

    ProtoString()
    {
    }

    char* c_str()
    {
        return data;
    }

    ProtoString operator+=(char c)
    {
        data[length] = c;
        length++;
        return *this;
    }

    ProtoString operator=(char* c)
    {
        length = 0;
        for (size_t i = 0; i < MAX_LENGTH; i++)
        {
           data[i] = 0;
        }
        
        while (*c != 0)
        {
            data[length] = *c;
            length++;
            c++;
        }
        return *this;
    }

};

ProtoString passwd = ProtoString(); 





int value[] = {0b0010, 0, 1, 0, 0b10, 0, 0b1100, 0b0100, 0b1000, 0b0100, 0b1001 };
char* val_name[] = {
    "MODE",
    "CLEAR_H",
    "CLEAR_L",
    "RETURN_H",
    "RETURN_L",
    "INIT_H",
    "INIT_L",
    "H_H",
    "H_L",
    "I_H",
    "I_L"
};

bool rs[] = {false, false, false, false, false, false, false, true, true, true, true};

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
    static int count = 0;
    static int lastKey = KEYPAD_NONE;
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

