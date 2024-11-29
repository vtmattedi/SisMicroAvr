#include <lcd.h>
#include <avr/delay.h>



void pulse_enable()
{
    pinWrite(EN, HIGH);
    _delay_ms(1);
    pinWrite(EN, LOW);
    _delay_us(2);
}

void lcd_send_4bit(uint8_t data, bool rs)
{
    pinWrite(RS, rs);
    pinWrite(D4, (data & 0b0001));
    pinWrite(D5, (data & 0b0010) >> 1);
    pinWrite(D6, (data & 0b0100) >> 2);
    pinWrite(D7, (data & 0b1000) >> 3);
    pulse_enable();
    _delay_ms(5);
}

void lcd_command(uint8_t data, bool rs)
{
    pinWrite(RS, rs);
    
    if (MODE_4_BIT)
    {
        lcd_send_4bit(data >> 4, rs);
        lcd_send_4bit(data & 0x0F, rs);
    }
    else 
    {
        //not implemented
    }
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    static uint8_t offsets[] = { 0x00, 40, 20, 60 };//Fucking try and error to find these values
  lcd_command(0x80 | (col + offsets[row]), false);
}

void clear_lcd()
{
    lcd_command(0x01, false);
}

void set_4bit_mode()
{
    lcd_send_4bit(0b0010, false);
}

void return_home()
{
    lcd_command(0x02, false);
}

void lcd_init_raw()
{
    pinMode(EN, OUTPUT);
    pinMode(RS, OUTPUT);
    pinMode(D4, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);
    pinMode(D7, OUTPUT);
    pinWrite(EN, LOW);
    pinWrite(RS, LOW);
    pinWrite(D4, LOW);
    pinWrite(D5, LOW);
    pinWrite(D6, LOW);
    pinWrite(D7, LOW);
    _delay_ms(15);
    
}

void print(char* str)
{
    while (*str)
    {
        lcd_command(*str, true);
        str++;
    }
}

void lcd_init()
{
    pinMode(EN, OUTPUT);
    pinMode(RS, OUTPUT);
    pinMode(D4, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);
    pinMode(D7, OUTPUT);
    pinWrite(EN, LOW);
    pinWrite(RS, LOW);
    pinWrite(D4, LOW);
    pinWrite(D5, LOW);
    pinWrite(D6, LOW);
    pinWrite(D7, LOW);
    _delay_ms(2);
    if (MODE_4_BIT)
        set_4bit_mode();
    clear_lcd();
    return_home();
    lcd_command(0x0C, false); // Display on, cursor off
    print("Hello World");
    _delay_ms(3);
}