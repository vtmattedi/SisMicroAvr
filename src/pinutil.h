#include <avr/io.h>

#define _portb 0
#define _portc 1
#define _portd 2
#define OUTPUT 1
#define INPUT 0
#define HIGH 1
#define LOW 0

void pinMode(int pin, bool mode);
void pinWrite(int pin, bool value);
bool pinRead(int pin);