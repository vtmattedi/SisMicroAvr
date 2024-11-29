#include <pinutil.h>

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
