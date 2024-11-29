#include <timing.h>

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

void init_millis(unsigned long f_cpu)
{
  unsigned long ctc_match_overflow;

  ctc_match_overflow = ((f_cpu / 1000) / 8); //when timer1 is this value, 1ms has passed

  // (Set timer to clear when matching ctc_match_overflow) | (Set clock divisor to 8)
  TCCR1B |= (1 << WGM12) | (1 << CS11);

  // high byte first, then low byte
  OCR1AH = (ctc_match_overflow >> 8);
  OCR1AL = ctc_match_overflow;

  // Enable the compare match interrupt
  TIMSK1 |= (1 << OCIE1A);

  //REMEMBER TO ENABLE GLOBAL INTERRUPTS AFTER THIS WITH sei(); !!!
  sei();
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



//Calculate the date and time from the seconds
void DateTime::Calculate(){
  unsigned long secs = seconds();
  //Basic Calcs
  this.second = secs % 60;
  this.minute = (secs / 60) % 60;
  this.hour = (secs / 3600) % 24;
 
  // Calculate the dow
  this.day_of_week = (secs / 86400L + 4) % 7; // 86400 seconds in a day, 4 is Thursday

//first done in a separeted function
// but most of the code is the same, so I put it here
// should be a sepetate method inside the strcture, but i'm optimizing for my time

  // Calculate the number of years
   int year = 1970;
   bool leap = false;
   int _secs_yr = secs;
   //while at least one year is left
    while (_secs_yr >= 31536000L){
      leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
      if (leap){
        if (_secs_yr >= 31622400L){
          _secs_yr -= 31622400L;
        } else {
            break;
        }
    } else {
    _secs_yr -= 31536000L;
    year++;
    }
  }
  this.year = year;

  // Calculate the number of days and months
  // days on each month
  int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (leap){
    days_in_month[1] = 29; // February has 29 days in a leap year
  }

  int day = 1;
  int month = 1;
  int _secs_mo = secs;
  while (_secs_mo >= 86400L) //while at least one day is left
  {
    for (int i = 0; i < 12; i++) {
      // Check if it's the last month
    if (_secs_mo >= days_in_month[i] * 86400L) {
      _secs_mo -= days_in_month[i] * 86400L;
      month++;
      if (month == 13){
        month = 1;
      }
    } else {
      day += _secs_mo / 86400L;
      break;
    }
  }
  }
  this.day = day;
  this.month = month;

}

char* DateTime::Date(){
  char date[11];
  sprintf(date, "%02d/%02d/%04d", this.day, this.month, this.year);
  return date;
}

char* DateTime::Time(){
  char time[9];
  sprintf(time, "%02d:%02d", this.hour, this.minute);
  return time;
}

char* DateTime::DateAndTime(){
  char datetime[20];
  sprintf(datetime, "%02d/%02d/%04d %02d:%02d", this.day, this.month, this.year, this.hour, this.minute);
  return datetime;
}