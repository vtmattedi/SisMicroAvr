#include <pinutil.h>


//Pin 0-7 are on PORTD
//Pin 8-13 are on PORTB (0- 5)
//PORTC 0-5 are the Analog pins (pins 14-19)

//Gets the port of an Arduino Pin
uint8_t getPort(int pin)
{
    if (pin >= 0 && pin <= 7)
    {
        return _portd;
    }
    else if (pin >= 8 && pin <= 13)
    {
        return _portb;
    }
    else if (pin >= 14 && pin <= 19)
    {
        return _portc;
    }
    return INVALID_PIN;
}

//Gets the pin number on the port of an Arduino Pin
uint8_t getPin(int pin)
{
    if (pin >= 0 && pin <= 7)
    {
        return pin;
    }
    else if (pin >= 8 && pin <= 13)
    {
        return pin - 8;
    }
    else if (pin >= 14 && pin <= 19)
    {
        return pin - 14;
    }
    return INVALID_PIN;
}

//Sets the DDRx register for a pin for input or output
void pinMode(int pin, bool mode)
{
    int port = getPort(pin);
    pin = getPin(pin);
    if (port == INVALID_PIN)
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

//Writes a value to a pin on PORTx based on Arduino pin number
void pinWrite(int pin, bool value)
{
    int port = getPort(pin);
     pin = getPin(pin);
    if (port == INVALID_PIN)
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

//Reads a value from a pin on PORTx based on Arduino pin number
bool pinRead(int pin)
{
     int port = getPort(pin);
     pin = getPin(pin);

    if (port == INVALID_PIN)
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
