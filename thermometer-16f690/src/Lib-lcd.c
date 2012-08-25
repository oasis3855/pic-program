/*
 * File:   Lib-lcd.c
 * Author: INOUE Hirokazu
 *
 * Created on 2012/08/26
 */

#include <stdio.h>
#include <stdlib.h>

#include <xc.h>
#include"Lib-lcd.h"

#ifndef _XTAL_FREQ
	/* 例：4MHzの場合、4000000 をセットする */
	#define _XTAL_FREQ 4000000
#endif

/* LCD_RS : データ制御ポート */
#define	LCD_RS	PORTCbits.RC4

/* LCD_R/W : Read/Write選択ポート */
#define	LCD_RW	PORTCbits.RC5

/* LCD_E : Eneble信号（この信号をHiにしている間、RS,R/W,Dataの送受信が行われる） */
#define LCD_EN	PORTCbits.RC6

/* データポート： LCD_DB4=RC0, LCD_DB5=RC1, LCD_DB6=RC2, LCD_DB7=RC3 */
#define LCD_DATA	PORTC

/* ENピンの信号を一瞬プルアップしてデータ転送 */
#define	LCD_STROBE()	((LCD_EN = 1),(LCD_EN = 0))

/* 4ビットモードでの8ビット分のデータ転送 */
void lcd_write(unsigned char c)
{
	__delay_us(40);
	LCD_DATA = (LCD_DATA & 0xf0) | ((c >> 4) & 0x0f);
	LCD_STROBE();
	LCD_DATA = (LCD_DATA & 0xf0) | (c & 0x0f);
	LCD_STROBE();
}

/* 画面クリア */
void lcd_clear(void)
{
	LCD_RS = 0;
	lcd_write(0x1);
	__delay_ms(2);
}

/* カーソル位置に文字列を出力 */
void lcd_puts(const char * s)
{
	LCD_RS = 1;	// write characters
	while(*s)
		lcd_write(*s++);
}

/* カーソル位置に1文字出力 */
void lcd_putch(char c)
{
	LCD_RS = 1;	// write characters
	lcd_write( c );
}


/* カーソル位置の設定 */
void lcd_goto(unsigned char pos)
{
	LCD_RS = 0;
	lcd_write(0x80+pos);
}

/* LCDを4ビットモードで初期化 */
void lcd_init()
{
	LCD_RS = 0;
	LCD_EN = 0;
	LCD_RW = 0;

	__delay_ms(15);			/* 15ミリ秒待つ */
	LCD_DATA = 0b00000011;	/* 一旦8bitモード設定（初期化） */
	LCD_STROBE();
	__delay_ms(5);
	LCD_STROBE();
	__delay_us(200);
	LCD_STROBE();
	__delay_us(200);
	LCD_DATA = 0b00000010;	/* 4bitモード設定 */
	LCD_STROBE();

	lcd_write(0x28);		/* 2行表示モードに設定 */
	lcd_write(0x0C);		/* 表示ON,カーソルOFF,カーソル点滅OFF */
	lcd_clear();
	lcd_write(0x6);			/* エントリーモード：左から右へ、シフト無し */
}
