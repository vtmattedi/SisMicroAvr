
#include <serialutil.h>

/// @brief Initialize the serial port at 9600 baud
void serialBegin()
{
    //Sets the baud rate
    UBRR0H = (MYUBRR >> 8);
    UBRR0L = MYUBRR;
    
    //Enable the receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // Set frame format: 8data, 2stop bit
    // pretty common configuration
    UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}
// Prints a string to the serial port
void serialPrint (char str[])
{
    int len = strlen(str);

    for (int i = 0; i < len; i++)
    {
        // Wait for the transmit buffer to be empty
        while (!(UCSR0A & (1 << UDRE0)));
        // Put the data into the buffer
        UDR0 = str[i];
    } 
}


char serialReadChar ()
{
    // Wait for data to be received
    while (!(UCSR0A & (1 << RXC0)));
    // Get and return received data from buffer
    return UDR0;
}

// this is very raw and should not be used
// but we need to develop our own read strategies
// for the project
void serialRead(char* str, int len)
{
    int i = 0;
    bool done = false;
    if (serialAvailable())
    {
    while (!done)
    {
        str[i] = serialReadChar();
        i++;
        if (str[i] == 0 || i >= len - 2)
        {
            // if it's the last character, add a null terminator
            if (i == len -1 )
            {
                str[len] = 0;
            }
            done = true;
        }
    }
    }
}

bool serialAvailable()
{
    // Check if the receive buffer has data
    return UCSR0A & (1 << RXC0);
}