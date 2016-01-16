/*
 * 粉塵センサーGP2Y1010AU0F サンプルアプリ
 *
 * GNU GPL Free Software (C) INOUE Hirokazu
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <float.h>
#include "i2c-lib.h"
#include "i2c-lcdlib-mjkdz.h"


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
//      if(flag_serial_enable) rs232c_putch(ch);
        return;
}

#if 0
void single_measure(unsigned int *value_max, unsigned int *value_min) {
        // 30回測定して、最大値を測定結果として画面表示する
        for(unsigned short int i = 0; i < 30; i++){
            // GP2Y1010AU0FのLEDをONにする320usの間に、1回 A/D変換して電圧値を得る処理

            ADCON0 = 0b00000001;            // AN0チャンネル選択, A/D機能ON
            PORTAbits.RA5 = 1;              // 測定LED ON
            __delay_us(280);                // A/D変換器チャージ時間10usを含み280us待つ
            ADCON0 = 0b00000011;            // AN0チャンネル選択, A/D開始, A/D機能ON
            while(ADCON0bits.GO_nDONE){}    // A/D変換完了を待つ
            unsigned int value = ADRESH << 8 | ADRESL;
            ADCON0 = 0;                     // AN0選択, A/D機能停止
            PORTAbits.RA5 = 0;              // 測定LED OFF

            if(value > *value_max) *value_max = value;
            if(*value_min > value) *value_min = value;

            // GP2Y1010AU0Fの最低測定間隔は10msのため、残り9.7ms待つ
            __delay_us(9700);

        }

}
#endif

void multi_measure(unsigned int *value_max, unsigned int *value_min) {
        // 30回測定して、最大値を測定結果として画面表示する
        for(unsigned short int i = 0; i < 30; i++){
            // GP2Y1010AU0FのLEDをONにする320usの間に、1回 A/D変換して電圧値を得る処理


            PORTAbits.RA5 = 1;              // 測定LED ON
            for(unsigned short int j = 0; j < 4; j++){
                ADCON0 = 0b00000001;            // AN0チャンネル選択, A/D機能ON
                __delay_us(7);                 // A/D変換器チャージ時間4.97us以上待つ
                ADCON0 = 0b00000011;            // AN0チャンネル選択, A/D開始, A/D機能ON
                while(ADCON0bits.GO_nDONE){}    // A/D変換完了を待つ
                unsigned int value = ADRESH << 8 | ADRESL;
                ADCON0 = 0;                     // AN0選択, A/D機能停止
                __delay_us(1);                  // 時間調整

                if(value > *value_max) *value_max = value;
                if(*value_min > value) *value_min = value;
            }
            PORTAbits.RA5 = 0;              // 測定LED OFF

            // GP2Y1010AU0Fの最低測定間隔は10msのため、残り9.7ms待つ
            __delay_us(9700);

        }

}



int main(int argc, char** argv) {
    // 基本機能の設定
    OSCCON = 0b01101010;        // 内部オシレーター 4MHz
    TRISA = 0b00001111;         // IOポートRA0(AN0),RA1(SCL),RA2(SDA)を入力モード（RA3は入力専用）、RA4(未使用),RA5(SensorLED)を出力モード
    ANSELA = 0b00000001;        // A/D変換をAN0を有効、AN1,AN2,AN4を無効
    PORTA = 0;

    ADCON0 = 0;                 // AN0選択, A/D機能停止
    ADCON1 = 0b10010000;        // 変換結果右詰, クロックFOSC/8, 比較対象VDD

    i2c_enable();
    OPTION_REGbits.nWPUEN = 0;  // I2C プルアップ抵抗 有効
    WPUA |= 0b00000110;          // pull-up (RA1=SCL, RA2=SDA pull-up enable)

    i2c_lcd_init();
    i2c_lcd_set_cursor_pos(0);
    printf("wait...");

    PORTAbits.RA5 = 0;
    // 1秒待つ（GP2Y1010AU0F仕様書による指定）
    __delay_ms(1000);
    i2c_lcd_clear();


    while(1){
        unsigned int value_max = 0, value_min = 0xffff;      // 検出電圧の最大・最小値
        float v_max = 0, v_min = 0;     // 換算した実電圧
        float v_dust = 0;


        multi_measure(&value_max, &value_min);

        v_max = (float)value_max * 5.0 / 1024.0;
        v_min = (float)value_min * 5.0 / 1024.0;
        v_dust = (v_max - 0.78) / 0.0063;
        i2c_lcd_set_cursor_pos(0);
        printf("Dust %03dug/m3", (int)v_dust);
        i2c_lcd_set_cursor_pos(0x40);
        printf("(max%03d min%03d)", (int)(v_max*100), (int)(v_min*100));


        __delay_ms(1000);
    }


    return (EXIT_SUCCESS);
}



