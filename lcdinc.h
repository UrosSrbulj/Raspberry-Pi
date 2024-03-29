#ifndef LCDINC_H
#define LCDINC_H

#include <wiringPi.h>

// Deklaracije funkcija
void lcd_strobe(void);
void lcd_write(unsigned char c, int mode);
void lcd_clear(void);
void lcd_home(void);
void lcd_cursor_blink_on(void);
void lcd_cursor_blink_off(void);
void lcd_cursors_off(void);
void lcd_cursors_on(void);
void lcd_puts(const char *s);
void lcd_putch(unsigned char c);
void lcd_goto(int col, int row);
void lcd_init(void);

#endif // LCDINC_H
