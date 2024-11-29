#include <avr/io.h>
//Some defines to make the code more readable
#define INVALID_PIN -1
#define _portb 0
#define _portc 1
#define _portd 2
#define OUTPUT 1
#define INPUT 0
#define HIGH 1
#define LOW 0
// Here he registers used are simple
// PORTx is the register that controls the output of the pin
// DDRx is the register that controls if the pin is input or output
// PINx is the register that reads the value of the pin
// Wrote some functions to make an Arduino pin usable for io mode and write/read
// The functions are simple and should be self-explanatory
// There are 20 valid pins in the Arduino Hardware
// PORTD 0-7 -> pins 0-7
// PORTB 0-5 -> pins 8-13
// PORTC 0-5 -> pins 14-19
// PORTB 6 and 7 and PORTC 6 and 7 are used internally by the Arduino Hardware
// and are not exposed. therefore there is no way to acess them in this code.
// The point is to use an Arduino but without the Arduino framework using only
// AVR's C/C++ libraries
void pinMode(int pin, bool mode);
void pinWrite(int pin, bool value);
bool pinRead(int pin);