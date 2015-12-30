/* 
 * DHT11（温湿度センサー）の読み出し サンプルアプリ
 * 
 * GNU GPL Free Software (C) INOUE Hirokazu
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xc.h>
#include "i2c-lib.h"
#include "i2c-lcdlib-mjkdz.h"
#include "dht11.h"

/* PIC Configuration 1 */
__CONFIG(FOSC_INTOSC &  // INTOSC oscillator: I/O function on CLKIN pin
        WDTE_OFF &      // WDT(Watchdog Timer) disabled
        PWRTE_ON &      // PWRT(Power-up Timer) disabled
        MCLRE_OFF &     // MCLR pin function is digital input
        CP_OFF &        // Program memory code protection is disabled
        CPD_OFF &       // Data memory code protection is disabled
        BOREN_OFF &     // BOR(Brown-out Reset) disabled
        CLKOUTEN_OFF &  // CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin
        IESO_OFF &      // Internal/External Switchover mode is disabled
        FCMEN_OFF);     // Fail-Safe Clock Monitor is disabled

/* PIC Configuration 2 */
__CONFIG(WRT_OFF &      // Flash Memory Self-Write Protection : OFF
        VCAPEN_OFF &    // VDDCORE pin functionality is disabled
        PLLEN_OFF &     // 4x PLL disabled
        STVREN_ON &     // Stack Overflow or Underflow will not cause a Reset
        BORV_HI &       // Brown-out Reset Voltage Selection : High Voltage
        DEBUG_OFF &     // In-Circuit Debugger disabled, ICSPCLK and ICSPDAT are general purpose I/O pins
        LVP_OFF);       // Low-voltage programming : disable

#ifndef _XTAL_FREQ
    /* 例：4MHzの場合、4000000 をセットする */
    #define _XTAL_FREQ 4000000
#endif

/* printf関数の出力先のスタブ関数を定義する */
void putch(unsigned char ch){
        i2c_lcd_putch(ch);
        return;
}

int main(int argc, char** argv) {
    // 基本機能の設定
    OSCCON = 0b01101010;        // 内部オシレーター 4MHz
    TRISA = 0b00101110;         // IOポートRA1(SCL),RA2(SDA),RA5(RX)を入力モード（RA3は入力専用）、RA0(AN0),RA4(TX)を出力モード
    ANSELA = 0b00000000;        // A/D変換をAN0,AN1,AN2,AN4を無効
    PORTA = 0;

    // I2Cバス初期化（LCD接続に使う）
    i2c_enable();
    OPTION_REGbits.nWPUEN = 0;  // I2C プルアップ抵抗 有効
    WPUA |= 0b00000110;          // pull-up (RA1=SCL, RA2=SDA pull-up enable)

    // LCD画面初期化
    i2c_lcd_init();
    i2c_lcd_set_cursor_pos(0);
    printf("DHT11 initialize ...");

    // DHT11初期化（WPUA有効、1秒ウォームアップ待機）
    dht11_init_port();
    // データ格納領域
    unsigned char dht11_data[5] = {0};
    // LCD画面消去
    i2c_lcd_clear();

    // 温湿度データの連続読み出し
    for(;;){
        // データ格納領域初期化
        for(short int i=0; i<4; i++) dht11_data[i] = 0;
        // DHT11データ読み込みとチェックサム結果表示
        i2c_lcd_set_cursor_pos(0x40);
        if(dht11_read_value(&dht11_data)) printf("Data OK");
        else printf("Data NG");
        // データの画面表示
        i2c_lcd_set_cursor_pos(0);
        printf("%02X,%02X,%02X,%02X,%02X", dht11_data[0], dht11_data[1], dht11_data[2], dht11_data[3], dht11_data[4]);

        // 2.5秒待機
        __delay_ms(2000);
        i2c_lcd_set_cursor_pos(0x40);
        printf(".......");
        __delay_ms(500);
    }

    return (EXIT_SUCCESS);
}

