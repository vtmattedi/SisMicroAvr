
#include <serialutil.h>

/// @brief Initialize the serial port at 9600 baud
void serialBegin()
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
