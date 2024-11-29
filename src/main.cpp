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
#include <lock.h>

ProtoString passwd = ProtoString();

void DateTimeToString(DateTime *dt, char* buffer){
  sprintf(buffer, "[%02d:%02d:%02d] - ", dt->hour, dt->minute, dt->second);
}

void cnp()
{
    clear_lcd();
    lcd_set_cursor(0, 0);
    print("Enter password:");
    lcd_set_cursor(1, 0);
     print("      ");
    for (int i = 0; i < lockHandler.password.length; i++)
    {
        print("*");
    }

    if (lockHandler.alarmRaised)
    {
        lcd_set_cursor(2, 0);
        print("Alarm! pew pew!");
    }
    lcd_set_cursor(3, 0);
    print(lockHandler.locked?"      Open":"     Locked");
}
// Convert a string to an unsigned long using base 10
unsigned long strtoul( const char *str )
{
    unsigned long result = 0;
    while (*str >= '0' && *str <= '9')
    {
        result = result * 10 + (*str - '0');
        str++;
    }
    return result;
}

int atoi (const char * str)
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

void serialPrintf(const char* str, ...)
{
    va_list args;
    va_start(args, str);
    char buffer[256];
    vsprintf(buffer, str, args);
    serialPrint(buffer);
    va_end(args);
}

int main(void)
{
    // Setup the keypad
    setupKeyPad();
    // Setup the serial port
    serialBegin();
    lcd_init();
    init_time(F_CPU);//F_CPU defined by avr in util/delay.h
    sei(); // Enable global interrupts

    static int lastKey = KEYPAD_NONE;
    serialPrint("Please send current timestamp\n");
    DateTime dt;
    while (true)
    {
        int key = readKeypad();
        if (key != lastKey)
        {
            lastKey = key;
            if (key != KEYPAD_NONE)
            {
                lockHandler.HandleInput(key);
                cnp();
                dt.Calculate();
                char dateBuffer[50];
                DateTimeToString(&dt,dateBuffer);
                serialPrint(dateBuffer);
                serialPrint("Passwd: \"");
                serialPrint(lockHandler.password.data);
                serialPrint("\"\n");
                char buffer [50];
                sprintf(buffer, "Key: %c\n", keyMap[key]);
                serialPrint(buffer);
                
            }
        }

        if (data_available())
        {
            ProtoString str;
            initProtoString(&str);
            while (data_available())
            {
                char c = serial_read();
                if (c != 13){
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
            dt.Calculate();
            sprintf(buffer,"Time set to: %02d:%02d:%02d\n", dt.hour, dt.minute, dt.second);
            serialPrint(buffer);

        }
 
    }

    return 1;
}

