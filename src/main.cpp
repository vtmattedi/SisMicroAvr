#define __DELAY_BACKWARD_COMPATIBLE__ // for _delay_ms, not sure if needed
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
// Our files just for organization
#include <eeprom.h>      //EEPROM functions
#include <serialutil.h>  // Serial functions
#include <pinutil.h>     // Pin functions
#include <keyPad.h>      // Keypad functions
#include <protostring.h> // String Helper functions
#include <lock.h>        // Lock functions
#include <lcd.h>         // LCD functions
#include <timing.h>      // Timing functions

#define RELAYPIN 8
#define ALARMPIN 9

ProtoString passwd = ProtoString();

void DateTimeToString(DateTime *dt, char *buffer)
{
    sprintf(buffer, "[%02d:%02d:%02d] - ", dt->hour, dt->minute, dt->second);
}

// Clear and print; used to update the display
void cnp()
{
    clear_lcd();
    lcd_set_cursor(0, 0);

    if (lockHandler.state == REGISTERING)
    {
        char buffer[50];
        sprintf(buffer, "Register User %d:", lockHandler.numUsers);
        lcd_print(buffer);
        lcd_set_cursor(1, 0);
        lcd_print("      ");
        for (int i = 0; i < lockHandler.password.length; i++)
        {
            lcd_print("*");
        }
        lcd_set_cursor(2, 0);
        lcd_print(lockHandler.numUsers == MAX_USERS ? "Max users reached" : "Enter 4 digits");
        lcd_set_cursor(3, 0);
        lcd_print("C Clear | # Save");

        return;
    }
    else if (lockHandler.state == MESSAGE)
    {
        ProtoString msg[4];
        // Split the message into 4 lines
        if (lockHandler.message.length > 16)
        {
            for (int i = 0 ; i < 4; i++)
            {
                for (int j = 0; j < 16; j++)
                {
                    protoStringAppendChar(&msg[i], lockHandler.message.data[j + i * 16]);
                }
            }
        }
        else
        {
            protoStringAssign(&msg[0], lockHandler.message.data);
        }
        for (int i = 0; i < 4; i++)
        {
            lcd_set_cursor(i, 0);
            lcd_print(msg[i].data);
        }
    }
    else if (lockHandler.state == IDLE)
    {
        lcd_print("Enter password:");
        lcd_set_cursor(1, 0);
        // Print the password as asterisks, centralizing it
        lcd_print("      ");
        for (int i = 0; i < lockHandler.password.length; i++)
        {
            lcd_print("*");
        }

        if (lockHandler.alarmRaised)
        {
            lcd_set_cursor(2, 0);
            lcd_print("Alarm! pew pew!");
        }
        else if (lockHandler.message.length > 0)
        {
            lcd_set_cursor(2, 0);
            lcd_print(lockHandler.message.data);
        }
        lcd_set_cursor(3, 0);
        lcd_print(lockHandler.locked ? "     Locked" : "      Open");
    }
}
// Convert a string to an unsigned long using base 10
unsigned long strtoul(const char *str)
{
    unsigned long result = 0;
    while (*str >= '0' && *str <= '9')
    {
        result = result * 10 + (*str - '0');
        str++;
    }
    return result;
}

// Convert a string to an int, base 10
int atoi(const char *str)
{
    int res = 0;
    bool neg = false;
    if (*str == '-')
    {
        neg = true;
        str++;
    }
    for (int i = 0; str[i] != '\0'; i++)
        res = res * 10 + str[i] - '0';
    if (neg)
    {
        res = -res;
    }
    return res;
}

int main(void)
{

    // Setup the keypad
    setupKeyPad();
    // Setup the serial port
    serialBegin();
    lcd_init();
    init_time(F_CPU); // F_CPU defined by avr in util/delay.h; Using 16MHz on the simluation.
    sei();            // Enable global interrupts

    // Setup the lock writing the password to the EEPROM
    // this only need to be done once
    // It could be done dynamically but for the sake of simplicity
    // we are doing it here ; Doing dynamically now.

    if (false)
    {
        static char *const passwd[] = {"1234", "4321", "4567", "9876", "0000"};
        for (int j = 0; j < 5; j++)
        {
            const char *data = passwd[j];
            for (int i = 0; i < 4; i++)
            {
                int addr = j * ADDR_MULTIPLIER + i;
                int res = EEPROM_write(addr, data[i]);
                if (res != EEPROM_OK)
                {
                    serialPrintf("Error writing to EEPROM: %d j: %d, i:%d\n", res, j, i);
                }
                // if (res == EEPROM_OK)
                // {
                //     serialPrintf("Wrote %c to EEPROM at %d\n", data[i], addr);
                // }
            }
        }
    }
    lockHandler.init(RELAYPIN, ALARMPIN);
    static int lastKey = KEYPAD_NONE;
    serialPrint("Please send current timestamp\n");
    while (true)
    {
        int key = readKeypad();
        if (key != lastKey)
        {
            lastKey = key;
            if (key != KEYPAD_NONE)
            {
                lockHandler.HandleInput(key);
                // cnp();
                // Prints current guess for debugging
                //  char dateBuffer[50];
                //  DateTimeToString(&dt, dateBuffer);
                //  serialPrint(dateBuffer);
                //  serialPrint("Passwd: \"");
                //  serialPrint(lockHandler.password.data);
                //  serialPrint("\"\n");
                //  char buffer[50];
                //  sprintf(buffer, "Key: %c\n", keyMap[key]);
                //  serialPrint(buffer);
            }
        }
        else
            lockHandler.HandleInput(KEYPAD_NONE); // call this to update the display

        if (lockHandler.stateChaged)
        {
            cnp();
            lockHandler.stateChaged = false;
        }

        // read serial data
        if (data_available())
        {
            ProtoString str;
            initProtoString(&str);
            while (data_available())
            {
                char c = serial_read();
                if (c != 13)
                {
                    protoStringAppendChar(&str, c);
                }
                // char buffer[50];
                // sprintf(buffer, "Received: %d\n", c);
                // serialPrint(buffer);
            }
            char buffer[100];
            sprintf(buffer, "Received: %s\n", str.data);
            serialPrint(buffer);
            unsigned long timestamp = strtoul(str.data);
            timestamp -= 10800; // we are in GMT -3
            set_seconds(timestamp);
            DateTime dt;
            dt.Calculate();
            sprintf(buffer, "Time set to: %02d:%02d:%02d\n", dt.hour, dt.minute, dt.second);
            serialPrint(buffer);
            sprintf(buffer, "Date set to: %02d/%02d/%02d\n", dt.day, dt.month, dt.year);
            serialPrint(buffer);
        }
    }

    return 1;
}
