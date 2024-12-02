
#include <serialutil.h>
#include <pinutil.h>
#include <util/atomic.h>
/// @brief Initialize the serial port at 9600 baud
void serialBegin()
{
    // Sets the baud rate
    // It is a 16-bit register, so we need to set the high and low bytes
    // and it will generate the baud rate for us using the internal clock
    UBRR0H = (MYUBRR >> 8);
    UBRR0L = MYUBRR;

    // Enable the receiver and transmitter. Also enable the receive interrupt
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    // Set frame format: 8data, 2stop bit
    // pretty common configuration
    UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}
// Prints a string to the serial port
void serialPrint(char str[])
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


// Prints a formatted string to the serial port
// using vsprintf
void serialPrintf(const char *str, ...)
{
    va_list args;
    va_start(args, str);
    char buffer[256];
    vsprintf(buffer, str, args);
    serialPrint(buffer);
    va_end(args);
}


//Recieves data from the serial port using interrupts
volatile static uint8_t rx_buffer[RX_BUFFER_SIZE] = {0};
volatile static uint16_t rx_count = 0;	
volatile static uint8_t uart_tx_busy = 1;
volatile static bool data_ready = false;

#define CR 13

ISR(USART_RX_vect){
	
	volatile static uint16_t rx_write_pos = 0;
	
	rx_buffer[rx_write_pos] = UDR0;
    // f*********** m************* SimulIDE does not f********
    // terminate its f******** strings. ffs. added a check for CR
    // cuz it does support sending CR.
    if (rx_buffer[rx_write_pos] == '\0' || rx_buffer[rx_write_pos] == CR){
        data_ready = true;
    }
	rx_count++;
	rx_write_pos++;
	if(rx_write_pos >= RX_BUFFER_SIZE){
		rx_write_pos = 0;
	}
	
}

// Returns the number of characters in the rxbuffer unread
uint16_t rx_data_count(void){
    int _count = 0;
    ATOMIC_BLOCK(ATOMIC_FORCEON){
        _count =  rx_count;
    }
	return _count;
}

bool data_available(void){
     ATOMIC_BLOCK(ATOMIC_FORCEON){
        return data_ready && rx_count > 0;
     }
}

// Reads a character from the rxbuffer
char serial_read(void){
	static uint16_t rx_read_pos = 0;
	uint8_t data = 0;
    ATOMIC_BLOCK(ATOMIC_FORCEON){
        data = rx_buffer[rx_read_pos];
	    rx_read_pos++;
	    rx_count--;
	    if(rx_read_pos >= RX_BUFFER_SIZE){
		    rx_read_pos = 0;
	    }
        if (rx_buffer[rx_read_pos] == '\0' || rx_buffer[rx_read_pos] == CR){
            data_ready = false;
        }
    }
	return data;
}

void serialPrintTime()
{
    DateTime dt;
    dt.Calculate();
    serialPrintf("[%02d:%02d %02d/%02d/%04d] - ", dt.hour, dt.minute, dt.day, dt.month, dt.year);
}