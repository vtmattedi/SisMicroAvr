#include <avr/io.h>
#include "eeprom.h"
#include <avr/interrupt.h>
#include <util/delay.h>

#define BAUD 9600
#define MYUBRR ((F_CPU/16/BAUD)-1)

/// @brief Initialize the serial port at 9600 baud
void SerialBegin()
{
    UBRR0H = (MYUBRR >> 8);
    UBRR0L = MYUBRR;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}
void serialPrint (char str[])
{
    int len = strlen(str);

    for (int i = 0; i < len; i++)
    {
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = str[i];
    } 
}

#define _portb 0
#define _portc 1
#define _portd 2
#define OUTPUT 1
#define INPUT 0
#define HIGH 1
#define LOW 0
//Pin 0-7 are on PORTD
//Pin 8-13 are on PORTB (0- 5)
//PC0-5 are the Analog pins
void pinMode(int pin, bool mode)
{
    int port = -1;
    if (pin >= 0 && pin <= 7)
    {
        port = _portd;
    }
    else if (pin >= 8 && pin <= 13)
    {
        port = _portb;
        pin -= 8;
    }
    else if (pin >= 14 && pin <= 19)
    {
        port = _portc;
        pin -= 14;
    }


    if (port == -1)
    {
        //Invalid pin
        return;
    }

    if (mode == OUTPUT)
    {
        switch (port)
        {
        case _portb:
            DDRB |= (1 << pin);
            break;
        case _portc:
            DDRC |= (1 << pin);
            break;
        case _portd:
            DDRD |= (1 << pin);
            break;
        }
    }
    else
    {
        switch (port)
        {
        case _portb:
            DDRB &= ~(1 << pin);
            break;
        case _portc:
            DDRC &= ~(1 << pin);
            break;
        case _portd:
            DDRD &= ~(1 << pin);
            break;
        }
    }
     
}

void pinWrite(int pin, bool value)
{
    int port = -1;
    if (pin >= 0 && pin <= 7)
    {
        port = _portd;
    }
    else if (pin >= 8 && pin <= 13)
    {
        port = _portb;
        pin -= 8;
    }
    else if (pin >= 14 && pin <= 19)
    {
        port = _portc;
        pin -= 14;
    }

    if (port == -1)
    {
        //Invalid pin
        return;
    }

    if (value)
    {
        switch (port)
        {
        case _portb:
            PORTB |= (1 << pin);
            break;
        case _portc:
            PORTC |= (1 << pin);
            break;
        case _portd:
            PORTD |= (1 << pin);
            break;
        }
    }
    else
    {
        switch (port)
        {
        case _portb:
            PORTB &= ~(1 << pin);
            break;
        case _portc:
            PORTC &= ~(1 << pin);
            break;
        case _portd:
            PORTD &= ~(1 << pin);
            break;
        }
    }
}

bool pinRead(int pin)
{
     int port = -1;
    if (pin >= 0 && pin <= 7)
    {
        port = _portd;
    }
    else if (pin >= 8 && pin <= 13)
    {
        port = _portb;
        pin -= 8;
    }
    else if (pin >= 14 && pin <= 19)
    {
        port = _portc;
        pin -= 14;
    }

    if (port == -1)
    {
        //Invalid pin
        return false;
    }

    switch (port)
    {
    case _portb:
        return (PINB & (1 << pin)) > 0;
    case _portc:
        return (PINC & (1 << pin)) > 0;
    case _portd:
        return (PIND & (1 << pin)) > 0;
    }
    return false;
}

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
int readKeypad()
{


    int res = KEYPAD_NONE;
    for (int i = 0; i < 4; i++)
    {
        pinWrite(2 + i ,HIGH);
        if (pinRead(COL1))
        {
            res = i * 3;
        }
        if (pinRead(COL2))
        {
            res = i * 3 + 1;
        }
        if (pinRead(COL3))
        {
            res = i * 3 + 2;
        }
    }

    if (res == 9)
    {
        return KEYPAD_STAR;
    }
    else if (res == 10)
    {
        return 0;
    }
    else if (res == 11)
    {
        return KEYPAD_HASH;
    }
    return res;
    //Set ROW1 high and read COL1
    //Set ROW1 low
    //Set ROW2 high and read COL1
    //Set ROW2 low
    //Set ROW3 high and read COL1
    //Set ROW3 low
    //Set ROW4 high and read COL1
    //Set ROW4 low
}

int main (void)
{
    //setup the keypad
    pinMode(ROW1,OUTPUT);
    pinMode(ROW2,OUTPUT);
    pinMode(ROW3,OUTPUT);
    pinMode(ROW4,OUTPUT);
    pinMode(COL1,INPUT);
    pinMode(COL2,INPUT);
    pinMode(COL3,INPUT);
    
    SerialBegin();
    while (1)
    {
        char* hw = "Hello World\n";
        serialPrint(hw);
        _delay_ms(1000);
    }

    return 1;
}