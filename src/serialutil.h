#include <avr/io.h>
#include "eeprom.h"
#include <avr/interrupt.h>
#include <util/delay.h>


#define BAUD 9600
// Calculating the UBRR value for 9600 baud rate
// Got the formula from the datasheet
//UBRR = USART Baud Rate Register
//UCR0A is the USART Control and Status Register A
// It has the RXC0 and TXC0 flags that indicate if there is data to be read or written
//UCR0B is the USART Control and Status Register B
// It has the RXEN0 and TXEN0 flags that enable the receiver and transmitter
//UCR0C is the USART Control and Status Register C
// It has the USBS0 and UCSZ00 flags that set the frame format
//UDR0 is the USART I/O Data Register
// It holds the data to be read or written
#define MYUBRR ((F_CPU/16/BAUD)-1)

void serialBegin();
void serialPrint (char str[]);
void serialRead(char* str);
bool serialAvailable();