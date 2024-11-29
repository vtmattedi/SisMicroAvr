
#include "millis.h"

#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>


/* ATOMIC_BLOCK
  Creates a block of code that is guaranteed to be executed
    atomically. Upon entering the block the Global Interrupt Status
    flag in SREG is disabled, and re-enabled upon exiting the block
    from any exit path.

    Two possible macro parameters are permitted, ATOMIC_RESTORESTATE
    and ATOMIC_FORCEON.
*/
volatile unsigned long timer1_millis;
//NOTE: A unsigned long holds values from 0 to 4,294,967,295 (2^32 - 1). It will roll over to 0 after reaching its maximum value.
volatile unsigned long timer1_secs;
ISR(TIMER1_COMPA_vect);
void init_millis(unsigned long f_cpu);
unsigned long millis (void);