/*
 * File:   serial-lib.c   for Microchip PIC XC8
 *
 * PIC 12F675 software sirial
 */

#include <stdio.h>
#include <stdlib.h>

#include <xc.h>
#include "serial-lib.h"

#ifndef _XTAL_FREQ
	/* 例：4MHzの場合、4000000 をセットする */
	#define _XTAL_FREQ 4000000
#endif

/* 2400bpsのタイミングは416マイクロ秒。この関数内でのwaitは360から380の間で確認済み */
/* 9600bpsのタイミングは104マイクロ秒。この関数内でのwaitは55から57の間で確認済み */
#define DELAY_WAIT_SEC  56


/* RS-232C送信：1文字（1バイト）送信する */
/* stopbit=1bit, parity=off */
/* __delay でタイミングを取る方式 */
void rs232c_send_char_delay(unsigned char ch){
    /* start bit */
    GPIObits.GP1 = 0; /* TX */
    __delay_us(DELAY_WAIT_SEC);

    for(unsigned int i=0; i<8; i++){
        if(ch>>i & 0x01){
            GPIObits.GP1 = 1; /* TX */
        }
        else{
            GPIObits.GP1 = 0; /* TX */
        }
        __delay_us(DELAY_WAIT_SEC);
    }

    /* stop bit */
    GPIObits.GP1 = 1; /* TX */
    __delay_us(DELAY_WAIT_SEC);
    __delay_us(10);
}

/* RS-232C送信：1文字（1バイト）送信する */
/* 2400bps, stopbit=1bit, parity=off */
/* Timer1でタイミングを取る方式 */
void rs232c_send_char_timer(unsigned char ch){
    T1CON = 0b00010001; /* 00-01-000-1 : Timer1設定を1:2プリスケール, Timer ON */
    /* (1/4MHz)*4*2PreScale = 2 micro-sec */
    /* 2400bpsのタイミングは416マイクロ秒。416/2 = 208タイマーカウント */
    /* 65536 - 208 = 65328 = 0xff30 この値が理論値だが、この関数内では190から205カウント（0xff42から0xff33）が許容値 */
    TMR1H = 0xff; /* 0xff3a >> 8 */
    TMR1L = 0x3a; /* 0xff3a & 0x00ff */
    PIE1bits.TMR1IE = 0; /* Timer1割り込みを無効化 */
    PIR1bits.TMR1IF = 0; /* Timer1カウンターオーバーフローフラグをクリア */

    /* start bit */
    GPIObits.GP1 = 0; /* TX */
    while(!PIR1bits.TMR1IF) ; /* Timer1のカウントが完了するのを待つ(416マイクロ秒) */

    for(unsigned int i=0; i<8; i++){
        TMR1H = 0xff; /* 0xff3a >> 8 */
        TMR1L = 0x3a; /* 0xff3a & 0x00ff */
        PIR1bits.TMR1IF = 0; /* Timer1再スタート */
        if(ch>>i & 0x01){
            GPIObits.GP1 = 1; /* TX */
        }
        else{
            GPIObits.GP1 = 0; /* TX */
        }
        while(!PIR1bits.TMR1IF) ; /* Timer1のカウントが完了するのを待つ(416マイクロ秒) */
    }

    TMR1H = 0xff; /* 0xff3a >> 8 */
    TMR1L = 0x3a; /* 0xff3a & 0x00ff */
    PIR1bits.TMR1IF = 0; /* Timer1再スタート */
    /* stop bit */
    GPIObits.GP1 = 1; /* TX */
    while(!PIR1bits.TMR1IF) ; /* Timer1のカウントが完了するのを待つ(416ミリ秒) */

    T1CONbits.TMR1ON = 0; /* Timer1停止 */
    __delay_us(10);

}

/* 1Byte 転送する（PIC -> PC) */
void rs232c_putch(const char c)
{
	rs232c_send_char_delay(c);
}

/* 文字列を転送する (PIC -> PC) */
void rs232c_puts(const char * s)
{
	while(*s)
		rs232c_putch(*s++);
}

void rs232c_test_pattern(void){
    for(;;){
        rs232c_puts("Test Pattern from PIC12F675\r\n");
        for(unsigned char i=0x20; i<=0xd0; i+=0x10){
            if(i==0x80||i==0x90) continue;
//            printf("%02X : ", i); /* 出力はputch()スタブ関数 */
            for(unsigned int j=0; j<=0xf; j++){
                rs232c_putch(i+j);
            }
            rs232c_puts("\r\n");
        }
        rs232c_puts("\r\n");
        __delay_ms(500);
    }
}
