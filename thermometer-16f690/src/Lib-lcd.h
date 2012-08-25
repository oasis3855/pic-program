/*
 * File:   Thermometer-main.c
 * Author: INOUE Hirokazu
 *
 * Created on 2012/08/26
 */
/*
 * File:   Lib-lcd.h
 * Author: INOUE Hirokazu
 *
 * Created on 2012/08/26
 */

#ifndef LIB_LCD_H
#define	LIB_LCD_H

#ifdef	__cplusplus
extern "C" {
#endif

void lcd_write(unsigned char c);
void lcd_clear(void);
void lcd_puts(const char * s);
void lcd_putch(char c);
void lcd_goto(unsigned char pos);
void lcd_init(void);

#ifdef	__cplusplus
}
#endif

#endif	/* LIB_LCD_H */

