/*
 * File:   Lib-rs232c.c
 * Author: INOUE Hirokazu
 *
 * Created on 2012/08/26
 */

#include <stdio.h>
#include <stdlib.h>

#include <xc.h>
#include"Lib-rs232c.h"

/* 1行受信完了フラグ：キャラクタ受信後 CR(0x0d)が受信されたとき1がセットされる */
int flag_rs232c_received;		// ■■ 受信機能を使わない場合はコメントアウト
/* 受信バッファ (PC -> PIC) */
char rs232c_buffer[20];			// ■■ 受信機能を使わない場合はコメントアウト

/* 割り込みコールバック関数（受信を行わない場合は、この関数をコメントアウトする） */
// ■■ 受信機能を使わない場合はコメントアウト
/******
void interrupt interrupt_callback(void)
{
	// RCIF:1バイト受信割り込みの場合
	if(PIR1bits.RCIF == 1)
	{
		char received_char = RCREG;		// 受信データを取り出す
		if(flag_rs232c_received == 1) return;	// フラグがクリアされていないときは受信データを無視する
		// 受信データが改行コードの場合
		if(received_char == (char)0x0d)
		{
			flag_rs232c_received = 1;
			return;
		}
		// 受信データを受信バッファに格納する（末尾NULLの文字列形式）
		for(int i=0; i<19; i++)
		{
			if(rs232c_buffer[i] == 0)
			{
				rs232c_buffer[i] = received_char;
				rs232c_buffer[i+1] = (char)0;
				break;
			}
		}
	}
	return;
}
*****/

/* EUSART (RS232C) 通信速度等の初期設定 */
void rs232c_init(void)
{
	// 通信条件の設定
	TXSTAbits.SYNC = 0;		// Asynchronous
	TXSTAbits.BRGH = 0;		// High Baud Rate Select bit : set to "Low speed"
	BAUDCTLbits.BRG16 = 0;	// 8-bit/Asynchronous case
	SPBRG = 25;				// 2400bps (on Fosc=4MHz, SYNC=0, BRGH=0, BRG16=0)
	RCSTAbits.CREN = 1;		// Continuous Receive Enable
	TXSTAbits.TX9 = 0;		// 8-bit transmission (Transmit)
	RCSTAbits.RX9 = 0;		// 8-bit transmission (Receive)
	// シリアルポートの有効化
	RCSTAbits.SPEN = 1;		// Serial port pins (RX/TX) enabled
	TXSTAbits.TXEN = 1;		// Transmit Enable

	// 受信用フラグのクリアと、格納文字列のクリア
	// ■■ 受信機能を使わない場合はコメントアウト
//	flag_rs232c_received = 0;
//	rs232c_buffer[0] = (char)0;

	// 割り込み（受信割り込みを利用する場合、RCIE=1 を設定する）
	PIE1bits.TXIE = 0;		// TX割り込み無効
	PIE1bits.RCIE = 0;		// RX割り込み有効		// ■■ 受信機能を使わない場合は0と設定
}

/* 1Byte 転送する（PIC -> PC) */
void rs232c_putch(char c)
{
	while(!TXIF)
	{
		continue;	/* EUSART送信バッファが空になるまで待つ */
	}
	TXREG = c;
	return;
}

/* 文字列を転送する (PIC -> PC) */
void rs232c_puts(const char * s)
{
	while(*s)
		rs232c_putch(*s++);
}
