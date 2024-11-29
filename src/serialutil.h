#include <avr/io.h>
#include "eeprom.h"
#include <avr/interrupt.h>
#include <util/delay.h>


#define BAUD 9600
#define MYUBRR ((F_CPU/16/BAUD)-1)

void serialBegin();
void serialPrint (char str[]);