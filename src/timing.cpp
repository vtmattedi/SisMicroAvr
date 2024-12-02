#include <timing.h>

volatile unsigned long timer1_millis;
volatile unsigned long timer1_secs;
// Max for unsigned long is 4,294,967,295 
// 4,294,967,295 / 1000 = 4294967 seconds
// 4294967 / 60 = 71582 minutes
// 71582 / 60 = 1193 hours
// 1193 / 24 = 49 days -> 49 days is the max time we can measure using millis()
// For seconds() we want to sync with the epoch time, so we can measure up to the year 2038
ISR(TIMER1_COMPA_vect)
{
  timer1_millis++;
  if (timer1_millis % 1000 == 0)
  {
    timer1_secs++;
  }
}

void init_time(unsigned long f_cpu)
{
  unsigned long ctc_match_overflow;

  ctc_match_overflow = ((f_cpu / 1000) / 8); // when timer1 is this value, 1ms has passed

  // TCCR1B is the Timer/Counter Control Register B
  //  Bit 2:0 – CS12:0: Clock Select

  // (Set timer to clear when matching ctc_match_overflow) | (Set clock divisor to 8)
  TCCR1B |= (1 << WGM12) | (1 << CS11);

  // OCR1A is the Output Compare Register A
  //  When the timer reaches this value, it will overflow

  // high byte first, then low byte
  OCR1AH = (ctc_match_overflow >> 8);
  OCR1AL = ctc_match_overflow;

  // TIMSK1 is the Timer/Counter Interrupt Mask Register
  // Bit 1: OCIE1A – Timer/Counter1, Output Compare A Match Interrupt Enable

  // Enable the compare match interrupt
  TIMSK1 |= (1 << OCIE1A);
}

// get the number of milliseconds since the program started
unsigned long millis(void)
{
  unsigned long millis_return;

  // Ensure this cannot be disrupted
  ATOMIC_BLOCK(ATOMIC_FORCEON)
  {
    millis_return = timer1_millis;
  }

  return millis_return;
}

// get the number of seconds since the program started
unsigned long seconds(void)
{
  unsigned long secs_return;

  // Ensure this cannot be disrupted
  ATOMIC_BLOCK(ATOMIC_FORCEON)
  {
    secs_return = timer1_secs;
  }
  return secs_return;
}

// set the number of seconds since the program started;
// this can be used to set the time
void set_seconds(unsigned long secs)
{
  // Ensure this cannot be disrupted
  ATOMIC_BLOCK(ATOMIC_FORCEON)
  {
    timer1_secs = secs;
  }
}


#define DAY 86400UL
#define YEAR 31536000UL

dateResult getDate(unsigned long secs)
{
  dateResult res;
  //unsigned long secs = seconds();
  //serialPrintf("Seconds: %lu\n", secs);
  int _year = secs / YEAR + 1970; // there is not enough leap years to make a difference
  //serialPrintf("Year: %d\n", _year);
  //That should be the full formula, however the max date is 2038, so we can simplify it
  //  int offsetDays = (_year - 1972) / 4 - (_year - 2000) / 100 + (_year - 2000) / 400;
  // 1972 is the first leap year after 1970, 2000 is the first century year after 1970, 2000 is the first 400 year after 1970
  // since 2038 - 1970 < 100, we can ignore the century rule and the 400 year rule
  int offsetDays = (_year - 1972) / 4 - 1 * (_year%4 == 0); // 1 for 1972 plus 1 for each leap year after 1972;
  //serialPrintf("Offset days: %d\n", offsetDays);
  unsigned long thisYearDays = (secs - ((_year - 1970) * YEAR + offsetDays * DAY)) / DAY;
  //serialPrintf("This year days: %lu\n", thisYearDays);

  //once again we can simplify
  bool leap = (_year % 4 == 0) ;
  unsigned long days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (leap)
  {
    days_in_month[1] = 29;
  }
  unsigned long currentMonth = 1;
  //serialPrintf("START: Month: %lu, Days: %lu\n, CurrentDaysLeft: %lu \n", currentMonth, days_in_month[currentMonth -1], thisYearDays);
  while (thisYearDays > days_in_month[currentMonth -1])
  {
    thisYearDays -= days_in_month[currentMonth -1];
    currentMonth++;
  //  serialPrintf("Month: %lu, Days: %lu\n, CurrentDaysLeft: %lu \n", currentMonth, days_in_month[currentMonth -1], thisYearDays);
  }
  res.year = _year;
  res.month = currentMonth;
  res.day = thisYearDays;
  return res;
}

// Calculate the date and time from the seconds
void DateTime::Calculate()
{
  unsigned long secs = seconds();
  // Basic Calcs
  this->second = secs % 60;
  this->minute = (secs / 60) % 60;
  this->hour = (secs / 3600) % 24;

  // Calculate the dow
  this->day_of_week = (secs / 86400L + 4) % 7; // 86400 seconds in a day, 4 is Thursday

  // Calculate the date
  dateResult date = getDate(secs);
  this->year = date.year;
  this->month = date.month;
  this->day = date.day;
}
