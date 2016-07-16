/*
 * 交流電流計  シリアル送信版
 * File:   main.c
 *
 * Created on 2016/07/17, 14:55
 */

/*
 * PIC 12F1822
 *   1 : VDD = 5.0V
 *   2 : RA5 = RX <---> HC06:TXD
 *   3 : RA4 = TX <---> HC06:RXD
 *   4 : RA3
 *   5 : RA2 = Status LED
 *   6 : RA1
 *   7 : RA0 = CT A/D
 *   8 : VSS = GND
 */


// PIC12F1822 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = HI        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), high trip point selected.)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "serial-lib.h"

#ifndef _XTAL_FREQ
    /* 例：4MHzの場合、4000000 をセットする */
    #define _XTAL_FREQ 4000000
#endif

// A/D サンプル数
#define AD_SAMPLE 2500
// データ送信の間の待ち時間（ミリ秒）
#define WAIT_MSEC 1510
// ステータスLED発光時間（ミリ秒）
#define STATUS_BLINK_MSEC 5


/* printf関数の出力先のスタブ関数を定義する */
void putch(unsigned char ch){
        rs232c_putch(ch);
        return;
}

void main(void) {
    // 基本機能の設定
    OSCCON = 0b01101010;        // 内部オシレーター 4MHz
    TRISA = 0b00101001;         // IOポートRA0(AN0),RA5(RX)を入力モード（RA3は入力専用）、RA1,RA2,RA4(TX)を出力モード
    APFCONbits.RXDTSEL = 1;     // シリアルポート RXをRA5ピンに割付
    APFCONbits.TXCKSEL = 1;     // シリアルポート TXをRA4ピンに割付
    ANSELA = 0b00000001;        // A/D変換をAN0を有効 AN1,AN2,AN4を無効
    PORTA = 0;
    ADCON0 = 0;                 // AN0選択, A/D機能停止
    ADCON1 = 0b10010000;        // 変換結果右詰, クロックFOSC/8, 比較対象VDD

    // 1秒待つ
    __delay_ms(1000);

    rs232c_init(9);             // init 9600bps

    TRISAbits.TRISA5 = 0;       // RA5(RX)は使わないので、消費電力低減のため出力モードにしておく
    
    while(1) {
        unsigned int value_max = 0, value_min = 0xffff;      // 検出電流の最大・最小値
        float value_sum = 0.0;              // 中間計算（合計値）
        for(unsigned int i = 0; i < AD_SAMPLE; i++){
            ADCON0bits.ADON = 1;            // A/D機能ON
            __delay_us(10);                 // A/D変換器チャージ時間待つ
            ADCON0bits.GO = 1;              // A/D開始
            while(ADCON0bits.GO_nDONE){}    // A/D変換完了を待つ
            unsigned int value = ADRESH << 8 | ADRESL;
            ADCON0 = 0;                     // AN0選択, A/D機能停止

            if(value > 512) value_sum += (value - 512);
            else value_sum += (512 - value);
            
            if(value > value_max) value_max = value;
            if(value_min > value) value_min = value;
            
            // 以上、ループ命令中のアセンブラ行数 約120行、1命令4クロック(1サイクル)なら、120*0.25us*4=120us (@ 4MHz)
            // A/Dチャージ待ちに10us
            // A/D変換にTAD=2.0us * 12bit = 24us (@ 4MHz, FOSC/8)
            __delay_us(10);
            // 以上、1ループで約160us掛かる →  6250 Hz
            // 60Hzの1周期は6250/60=104.2サンプル、50Hzなら6250/50=125サンプルとなる。
            // 2500回測定で、60Hzなら24周期分、60Hzなら20周期分の測定が出来る … 2500回 * 160us = 400ms
            
            // ※ 実稼働時にロジック・アナライザで実行時間を計測したところ、
            // 予想より実行時間が大幅に掛かることがわかったため、WAIT_MSEC を4秒になるよう調整した。
        }

        // サンプル数で割って平均値を求める。なお、0 - 1024のレンジに調整するため、sin正弦波平均値2/piを考慮し、
        // 1024 / (512 * 2/pi) を調整する
        int value_average = (int)((value_sum / AD_SAMPLE) * (1024 / 325));
        // 効率。最大振幅の綺麗なsin波だった場合の平均値2/piで割る(実際は352で割ればよい)
        int value_efficiency = (int)((value_sum / AD_SAMPLE) / ((value_max - value_min)*0.3185) * 100);
        // Status LED ON
        PORTAbits.RA2 = 1;
        // データ送信 (to Serial)
        printf("%d,%d,%d,%d\r\n", value_min, value_max, value_average, value_efficiency);
        __delay_ms(STATUS_BLINK_MSEC);
        // Status LED OFF
        PORTAbits.RA2 = 0;
        
        // 2秒に1回データ送信する
        __delay_ms(WAIT_MSEC);
    }

    return;
}
