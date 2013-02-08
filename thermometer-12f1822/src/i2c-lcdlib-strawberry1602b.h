/* 
 * File:   i2c-lcdlib-strawberry1602b.h
 * Author: vm
 *
 * Created on 2013/02/02, 8:59
 */

#ifndef I2C_LCDLIB_STRAWBERRY1602B_H
#define	I2C_LCDLIB_STRAWBERRY1602B_H

#ifdef	__cplusplus
extern "C" {
#endif

// LCD device I2C Address
#define LCD_I2C_ADDR	0x3e

// LCD Contrast (ST7032)
#define LCD_CONTRAST		0x28

void i2c_lcd_send_command(unsigned char data);
void i2c_lcd_send_data(unsigned char data);
void i2c_lcd_init(void);
void i2c_lcd_putch(const unsigned char c);
void i2c_lcd_puts(const unsigned char *s);
void i2c_lcd_cmd(const unsigned char cmd);
void i2c_lcd_clear(void);
void i2c_lcd_set_cursor_pos(const unsigned char pos);


#ifdef	__cplusplus
}
#endif

#endif	/* I2C_LCDLIB_STRAWBERRY1602B_H */

