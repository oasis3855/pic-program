/* 
 * File:   main.c    for Microchip PIC XC8
 *
 * PIC 12F675 thermometer
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#include "serial-lib.h"
#include "common-lib.h"

#pragma config BOREN = OFF      // Brown-out Detect Enable bit
#pragma config CPD = OFF        // Data Code Protection bit
#pragma config FOSC = INTRCIO   // Oscillator Selection bits
#pragma config MCLRE = OFF      // GP3/MCLR pin function select
#pragma config WDTE = OFF       // Watchdog Timer Enable bit
#pragma config CP = OFF         // Code Protection bit
#pragma config PWRTE = ON       // Power-Up Timer Enable bit

#ifndef _XTAL_FREQ
	/* 例：4MHzの場合、4000000 をセットする */
	#define _XTAL_FREQ 4000000
#endif


// RS232Cに数値を文字列化して出力（送信）する
void print_uint(unsigned int n, unsigned char digit){
    unsigned char buf[3];

    // 2ケタの10進数（表示データがない倍は空白文字表示する）
    if(digit == 2){
        rs232c_puts(uint_to_dec2(n, buf));
    }
    // 1桁の10進数（表示データがない倍はゼロ表示する）
    else{
        rs232c_puts(uint_to_dec1_0(n, buf));
    }
}


int main(int argc, char** argv) {

    TRISIO = 0b00011000;        // GP3,GP4をINPUT, その他はOUTPUT
    CMCON = 0x7;                // コンパレーターOFF
    ANSEL = 0b00011000;         // Fosc/8, AN3
    INTCON = 0;                 // 割り込み全てOFF
    GPIO = 0;
    GPIObits.GP1 = 1;           // RS232C TX=1（未送信時はプルアップしておく）


    __delay_ms(500);

    for(;;){
        // AN3入力をA/D変換
        ADCON0 = 0b10001101;        // 右詰め, AN3, A/D機能ON
        __delay_us(20);             // A/D変換器チャージ時間待つ
        ADCON0bits.GO_DONE = 1;     // A/D変換開始
        while(ADCON0bits.GO_nDONE){}    // A/D変換完了を待つ
        unsigned int ad_value = ADRESH << 8 | ADRESL;
        ADCON0 = 0;                 // A/D機能OFF

        // A/D変換値を、LM35で測定した気温に換算する
        GPIObits.GP0 = 1;            // LED点灯
        ad_value = (unsigned int)((unsigned long int)ad_value * 500L / 0x3ffL);

        // 気温データをシリアルポートに出力する
        print_uint(ad_value/10, 2);
        rs232c_putch('.');
        print_uint(ad_value%10, 1);
        rs232c_puts("\r\n");
        GPIObits.GP0 = 0;            // LED消灯

        __delay_ms(1000);
    }

//    return (EXIT_SUCCESS);
}

