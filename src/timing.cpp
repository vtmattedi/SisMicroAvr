#include <timing.h>
volatile unsigned long timer1_millis;
//NOTE: A unsigned long holds values from 0 to 4,294,967,295 (2^32 - 1). It will roll over to 0 after reaching its maximum value.
volatile unsigned long timer1_secs;
static unsigned long offset = 0;
/*
The millis() function known from Arduino
Calling millis() will return the milliseconds since the program started

Tested on atmega328p

Using content from http://www.adnbr.co.uk/articles/counting-milliseconds
Author: Monoclecat, https://github.com/monoclecat/avr-millis-function

REMEMBER: Add sei(); after init_millis() to enable global interrupts!
 */


ISR(TIMER1_COMPA_vect)
{
  timer1_millis++;
  if (timer1_millis%1000 == 0)
  {
    timer1_secs++;
  }
}

void init_time(unsigned long f_cpu)
{
  unsigned long ctc_match_overflow;

  ctc_match_overflow = ((f_cpu / 1000) / 8); //when timer1 is this value, 1ms has passed

  //TCCR1B is the Timer/Counter Control Register B
  // Bit 2:0 – CS12:0: Clock Select

  // (Set timer to clear when matching ctc_match_overflow) | (Set clock divisor to 8)
  TCCR1B |= (1 << WGM12) | (1 << CS11);

  //OCR1A is the Output Compare Register A
  // When the timer reaches this value, it will overflow

  // high byte first, then low byte
  OCR1AH = (ctc_match_overflow >> 8);
  OCR1AL = ctc_match_overflow;

  // TIMSK1 is the Timer/Counter Interrupt Mask Register
  // Bit 1: OCIE1A – Timer/Counter1, Output Compare A Match Interrupt Enable

  // Enable the compare match interrupt
  TIMSK1 |= (1 << OCIE1A);

}

//get the number of milliseconds since the program started
unsigned long millis (void)
{
  unsigned long millis_return;

  // Ensure this cannot be disrupted
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    millis_return = timer1_millis;
  }
  
  return millis_return;
}

//get the number of seconds since the program started
unsigned long seconds (void)
{
  unsigned long secs_return;

  // Ensure this cannot be disrupted
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    secs_return = timer1_secs;
  }
  return secs_return;
}

//set the number of seconds since the program started; 
//this can be used to set the time
void set_seconds (unsigned long secs)
{
  // Ensure this cannot be disrupted
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    timer1_secs = secs;
  }
}

 unsigned long seconds2()
{
  unsigned long secs = offset  + millis()/1000;
  return secs;
}

int daysInMonths(uint8_t month, bool leap)
{
  int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (month == 2 && leap)
  {
    return 29;
  }
  return days_in_month[month - 1];
}


struct dateResult
{
  int year = 1970;
  int month = 1;
  int day = 1;
};
#define DAY  60 * 60 * 24
#define YEAR  365 * DAY
#define LEAP_YEAR 366 * DAY

int leapYearsSince(int year)
{
  int count = 0;
  while (year > 1970)
  {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
    {
      count++;
    }
    year--;
  }
  return count;
}


dateResult getDate()
{
  dateResult res;
  unsigned long secs = seconds();
  int _year = secs/YEAR + 1970; //there is not enough leap years to make a difference
  int offsetDays = secs % YEAR;
  int thisYearDays = (_year - offsetDays) % YEAR;
  int _month = 1;
  bool leap = (_year % 4 == 0 && _year % 100 != 0) || (_year % 400 == 0);
  int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (leap)
  {
    days_in_month[1] = 29;
  }
  int totalDays = 0;
  int _day = 1;
  for (int i = 0; i < 12; i++)
  {
    totalDays += days_in_month[i] * DAY;
    if (totalDays > thisYearDays)
    {
      _month = i + 1;
      _day = (thisYearDays - (totalDays - days_in_month[i] * DAY)) / DAY;
        res.year = _year;
        res.month = _month;
        res.day = _day;
    }
  }
  res.year = _year;
  res.month = _month;
  res.day = _day;

}


int naiveYear()
{
  unsigned long secs = seconds();
  int _year = 1970;

  bool done = false;
  while (secs >= 31536000L && !done){
    bool leap = (_year % 4 == 0 && _year % 100 != 0) || (_year % 400 == 0);
    if (leap){
      if (secs >= 31622400L){
        secs -= 31622400L;
      } else {
         done = true;
      }
  } else {
    secs -= 31536000L;
    _year++;
    }
  }
  return _year;
}

int naiveMonth()
{
  unsigned long secs = seconds();
  int _month = 1;
  int _year = 1970;
  bool done = false;
  while (secs >= 86400L && !done){
     unsigned int days = daysInMonths(_month, _year) * 86400L;
    if (secs >= days ) {
      secs -= days;
      _month++;
      if (_month > 12){
        _month = 1;
        _year++;
      }
    } else {
      done = true;
    }
  }
  return _month;
}

int naiveDay()
{
  unsigned long secs = seconds();
  int _day = 1;
  int _year = 1970;
  int _month = 1;
  bool done = false;
  while (secs >= 86400L && !done){
    unsigned int days = daysInMonths(_month, _year) * 86400L;
    if (secs >= days){
      secs -= days;
      _month++;
      if (_month > 12){
        _month = 1;
        _year++;
      }
    } else {
      done = true;
    }
  }

  while (secs >= 86400L){
    _day++;
    secs -= 86400L;
  }
  return _day;
}




//Calculate the date and time from the seconds
void DateTime::Calculate(){
  unsigned long secs = seconds();
  //Basic Calcs
  this->second = secs % 60;
  this->minute = (secs / 60) % 60;
  this->hour = (secs / 3600) % 24;
 
  // Calculate the dow
  this->day_of_week = (secs / 86400L + 4) % 7; // 86400 seconds in a day, 4 is Thursday
  // dateResult date = getDate();
  // this->year = date.year;
  // this->month = date.month;
  // this->day = date.day;

//first done in a separeted function
// but most of the code is the same, so I put it here
// should be a sepetate method inside the strcture, but i'm optimizing for my time
  

}

