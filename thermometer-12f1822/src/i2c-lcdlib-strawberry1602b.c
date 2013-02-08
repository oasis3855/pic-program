/*
 * File:   i2c-lcdlib-strawberry1602b.c   for Microchip PIC XC8 (12F1822, 16F1823, 16F1827)
 *
 * I2C LCD, Strawberry Linux I2C低電圧キャラクタ液晶モジュール（１６ｘ２行）SB1602B用
 */

#include <stdio.h>
#include <stdlib.h>

#include <xc.h>
#include "i2c-lib.h"
#include "i2c-lcdlib-strawberry1602b.h"

#ifndef _XTAL_FREQ
    /* 例：4MHzの場合、4000000 をセットする */
    #define _XTAL_FREQ 4000000
#endif

// I2C Bus Control Definition
#define I2C_WRITE_CMD 0
#define I2C_READ_CMD 1

// ST7032 Control Byte
#define ST7032_CTRL_CO_H  0x80
#define ST7032_CTRL_CO_L  0
#define ST7032_CTRL_RS_H  0x40
#define ST7032_CTRL_RS_L  0

// ST7032 Power/Icon function set
#define ST7032_POWER_BOOST_H    0x04
#define ST7032_POWER_BOOST_L    0
#define ST7032_ICON_H   0x08
#define ST7032_ICON_L   0

// ST7032 Follower control
#define ST7032_FOLLOWER_H   0x08
#define ST7032_FOLLOWER_L   0
#define ST7032_V0_AMP       0x04

// LCD Command
#define LCD_CLEAR 0x01

// send command to I2C LCD (I2C LCDにコマンドデータを送信する)
void i2c_lcd_send_command(unsigned char data){
    i2c_start();
    i2c_send_byte((LCD_I2C_ADDR<<1) | I2C_WRITE_CMD);   // アドレスを1ビット左にシフトし、末尾にR/Wビット（Write=0）を付与
    i2c_send_byte(ST7032_CTRL_CO_L | ST7032_CTRL_RS_L);
    i2c_send_byte(data);
    i2c_stop();
}

// send data to I2C LCD (I2C液晶にデータを送信する)
void i2c_lcd_send_data(unsigned char data){
    i2c_start();
    i2c_send_byte((LCD_I2C_ADDR<<1) | I2C_WRITE_CMD);   // アドレスを1ビット左にシフトし、末尾にR/Wビット（Write=0）を付与
    i2c_send_byte(ST7032_CTRL_CO_L | ST7032_CTRL_RS_H);
    i2c_send_byte(data);
    i2c_stop();

    __delay_us(30);     // Wait 26.3 us
}

// I2C LCD initialize (I2C液晶の初期化)
void i2c_lcd_init(void){
    __delay_ms(50);     // Wait after power ON 40 ms

    // Function set : Sets to 8-bit operation, 2 line, IS=0
    i2c_lcd_send_command(0x38);
    __delay_us(30);     // Wait 26.3 us

    // Function set : Sets to 8-bit operation, 2 line, IS=1
    i2c_lcd_send_command(0x39);
    __delay_us(30);     // Wait 26.3 us

    // Internal OSC frequency = 192Hz@5.0V, Bias=1/5 (IS=1 mode)
    i2c_lcd_send_command(0x14);
    __delay_us(30);     // Wait 26.3 us

    // Contrast set(low byte)=0x8 (IS=1 mode)
    i2c_lcd_send_command(0x70 | (LCD_CONTRAST & 0x0f));
    __delay_us(30);     // Wait 26.3 us

    // Contrast set(high byte)=0x2, Icon=ON, Booster circuit=OFF (IS=1 mode)
    i2c_lcd_send_command(0x50 | ST7032_POWER_BOOST_L | ST7032_ICON_L | ((LCD_CONTRAST >> 4) & 0x03));
    __delay_us(30);     // Wait 26.3 us

    // follower circuit=ON, Amplified Ratio (IS=1 mode)
    i2c_lcd_send_command(0x60 | ST7032_FOLLOWER_H | ST7032_V0_AMP);
    __delay_ms(220);     // Wait 200 ms

    // Function set : Sets to 8-bit operation, 2 line, IS=0
    i2c_lcd_send_command(0x38);
    __delay_us(30);     // Wait 26.3 us

    // Display=ON, Cursor=OFF
    i2c_lcd_send_command(0x0c);
    __delay_us(30);     // Wait 26.3 us

    // Clear Display
    i2c_lcd_send_command(LCD_CLEAR);
    __delay_ms(2);     // Wait 1.08 ms

    // Entry Mode Set
    i2c_lcd_send_command(0x06);
    __delay_us(30);     // Wait 26.3 us
}

// put one character on LCD (LCDに1文字表示する)
void i2c_lcd_putch(const unsigned char c){
    i2c_lcd_send_data(c);                   //文字表示
}

// put string on LCD (LCDに文字列を表示する)
void i2c_lcd_puts(const unsigned char *s){
    while(*s)               //文字取り出し
        i2c_lcd_send_data(*s++);            //文字表示
}

// send command to I2C LCD (I2C液晶にコマンドを送信する)
void i2c_lcd_cmd(const unsigned char cmd){
    i2c_lcd_send_command(cmd);
    __delay_us(30);    // Wait 26.3 us (except CLEAR DISPLAY)
}

// clear screen of I2C LCD
void i2c_lcd_clear(void){
    i2c_lcd_send_command(LCD_CLEAR);
    __delay_ms(2);    // Wait 1.08 ms
}

// set cursor position of I2C LCD (I2C液晶 カーソル位置の指定)
void i2c_lcd_set_cursor_pos(const unsigned char pos){
    // line one : pos=0 ...
    // line two : pos=0x40 ...
    i2c_lcd_send_command(0x80 | pos);
    __delay_us(30);    // Wait 26.3 us
}
