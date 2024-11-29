#include <pinutil.h>

#define EN 2
#define RS 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7

#define MODE_4_BIT true
#define delay1_ms 3
#define pulse_delay_ms 1
#define fourbit_delay_ms 1

void lcd_init();
void lcd_init_raw();
void lcd_send_4bit(uint8_t data, bool rs);
void print(char* str);  
void clear_lcd();
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_command(uint8_t data, bool rs);