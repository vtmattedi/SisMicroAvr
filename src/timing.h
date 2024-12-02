#pragma once // same as #ifndef ... #define ... #endif construction

#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>
#include <serialutil.h>

/* ATOMIC_BLOCK
  Creates a block of code that is guaranteed to be executed
    atomically. Upon entering the block the Global Interrupt Status
    flag in SREG is disabled, and re-enabled upon exiting the block
    from any exit path.

    Two possible macro parameters are permitted, ATOMIC_RESTORESTATE
    and ATOMIC_FORCEON.
*/
extern volatile unsigned long timer1_millis;
//NOTE: A unsigned long holds values from 0 to 4,294,967,295 (2^32 - 1). It will roll over to 0 after reaching its maximum value.
extern volatile unsigned long timer1_secs;
ISR(TIMER1_COMPA_vect);
void init_time(unsigned long f_cpu);
unsigned long millis (void);
unsigned long seconds (void);
void set_seconds (unsigned long secs);

struct DateTime
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t day_of_week;
  void Calculate();
};



struct dateResult
{
  int year = 1970;
  int month = 1;
  int day = 1;
};

dateResult getDate(unsigned long secs);
