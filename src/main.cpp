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
#include <adc.h>         // ADC functions
#include <WaterHandler.h>// Handle Water Level  functions
#include <timing.h>      // Timing functions



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

    init_time(F_CPU); // F_CPU defined by avr in util/delay.h; Using 16MHz on the simluation.
    sei();            // Enable global interrupts

    // Setup the lock writing the password to the EEPROM
    // this only need to be done once
    // It could be done dynamically but for the sake of simplicity
    // we are doing it here ; Doing dynamically now.
 
    static int lastKey = KEYPAD_NONE;
    serialPrint("Please send current timestamp\n");
    while (true)
    {
        // read serial data
        if (data_available())
        {
            ProtoString str;
            while (data_available())
            {
                char c = serial_read();
                if (c != 13)
                {
                   str += c;
                }
                // char buffer[50];
                // sprintf(buffer, "Received: %d\n", c);
                // serialPrint(buffer);
            }
            if (str[0] == 't')
            {
                str = str.substring(2);
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
    }

    return 1;
}
