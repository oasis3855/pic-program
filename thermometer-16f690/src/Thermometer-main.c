/* 
 * File:   Thermometer-main.c
 * Author: INOUE Hirokazu
 *
 * Created on 2012/08/26
 */

#include <stdio.h>
#include <stdlib.h>

#include<xc.h>
#include"Lib-rs232c.h"
#include"Lib-lcd.h"

/* PIC Configuration */
__CONFIG(CP_OFF &		// Program memory code protection is disabled
		CPD_OFF &		// Data memory code protection is disabled
		BOREN_OFF &		// BOR(Brown-out Reset) disabled
		MCLRE_OFF &		// MCLR pin function is digital input
		WDTE_OFF &		// WDT(Watchdog Timer) disabled
		PWRTE_ON &		// PWRT(Power-up Timer) disabled
		FOSC_INTRCIO);	// INTOSCIO oscillator: I/O function on RA4 and RA5

#ifndef _XTAL_FREQ
	/* 例：4MHzの場合、4000000 をセットする */
	#define _XTAL_FREQ 4000000
#endif

/* printf関数の出力先のスタブ関数を定義する */
void putch(unsigned char ch){
	lcd_putch(ch);
	rs232c_putch(ch);
	return;
}

int main(int argc, char** argv) {

	// 基本機能の設定
	OSCCON = 0b01100000;	// 内部オシレーター 4MHz
	TRISA = 0b00001100;		// IOポートRA2(温度センサー),RA3(プッシュスイッチ)を入力モード
	TRISB = 0b00100000;		// IOポートRB4,RB6,RB7(TX)を出力モード、RB5(RX)を入力モード
	TRISC = 0;				// IOポートRC0...RC7を出力モード
	INTCONbits.PEIE = 0;	// 割り込み機能をOFF
	PORTA = 0;				// RA0...RA5をV-low
	PORTB = 0;				// RB4...RA7をV-low
	PORTC = 0;				// RC0...RC7をV-low
	CM1CON0 = 0;			// コンパレーター1 無効
	CM2CON0 = 0;			// コンパレーター2 無効
	ANSEL = 0b00000100;		// AN2 A/Dコンバータのみ有効
	ANSELH = 0;				// AN8...AN11 A/Dコンバータを無効
	ADCON1 = 0b00010000;	// ADコンバータ クロック Fosc/8

	// 0.5秒待つ
	__delay_ms(500);

	// LCD初期化
	lcd_init();
	lcd_goto(0);
	lcd_puts("Thermometer");
	lcd_goto(0x40);
	lcd_puts("PIC 16F690 and LM35");

	// RS232C初期化
	rs232c_init();
	rs232c_puts("\r\nThermometer start ...\r\n");

	// 1秒待つ
	__delay_ms(1000);

	// ■■ RS232C受信機能（割り込み）を使う場合に次の2行を有効化する
//	INTCONbits.PEIE = 1;	// 割り込み機能をON
//	INTCONbits.GIE = 1;		// 割り込み機能をON

	// 最低・最高温度の記憶
	unsigned long int value2_min = 500L;
	unsigned long int value2_max = 0L;

	for(;;)
	{
		PORTAbits.RA5 = 1;				// 動作中LEDを点灯

		int flag_ra3 = PORTAbits.RA3;	// プッシュボタンを検出

		ADCON0 = 0b10001001;			// 1-0-0010-0-1: 右詰め,AN2,AD有効化
		__delay_us(10);					// A/D変換器チャージ時間待つ
		ADCON0bits.GO = 1;				// A/D変換開始
		while(ADCON0bits.GO_DONE){}		// A/D変換完了を待つ
		unsigned int value = ADRESH << 8 | ADRESL;
		// 10bit ADの最大値0x3ffが5.0V、さらにLM35の10倍増幅値では50℃になるように変換
		unsigned long int value2 = value * 500L / 0x3ffL;
		if(value2 > value2_max) value2_max = value2;
		if(value2 < value2_min) value2_min = value2;
		ADCON0bits.ADON = 0;			// A/Dを一旦無効化（節電）

		lcd_clear();
		printf("%02lu.%01lu deg-C (%04X)", value2/10, value2%10, value);
		rs232c_puts("\r\n");
		lcd_goto(0x40);
		printf("Min %02lu.%01lu Max %02lu.%01lu", value2_min/10, value2_min%10, value2_max/10, value2_max%10);
		rs232c_puts("\r\n");

		// AD変換前後で連続してプッシュボタン押し下げを検知した場合、最小・最大値をリセット
		if(flag_ra3 == 0 && PORTAbits.RA3 == 0)
		{
			value2_min = value2;
			value2_max = value2;
		}

		PORTAbits.RA5 = 0;				// 動作中LEDを消灯

		// 5秒待つ
		for(int i=0; i<5; i++)
			__delay_ms(1000);
	}

}
