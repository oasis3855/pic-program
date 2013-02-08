/*
 * Temperature Sensor with EEPROM data save for Microchip PIC 12F1822
 * File:   main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <xc.h>
#include "common-lib.h"
#include "i2c-lib.h"
#include "i2c-lcdlib-strawberry1602b.h"
#include "i2c-eepromlib.h"
#include "serial-lib.h"

/* PIC Configuration 1 */
__CONFIG(FOSC_INTOSC &	// INTOSC oscillator: I/O function on CLKIN pin
		WDTE_OFF &		// WDT(Watchdog Timer) disabled
		PWRTE_ON &		// PWRT(Power-up Timer) disabled
		MCLRE_OFF &		// MCLR pin function is digital input
		CP_OFF &		// Program memory code protection is disabled
		CPD_OFF &		// Data memory code protection is disabled
		BOREN_OFF &		// BOR(Brown-out Reset) disabled
		CLKOUTEN_OFF &	// CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin
		IESO_OFF &		// Internal/External Switchover mode is disabled
		FCMEN_OFF);		// Fail-Safe Clock Monitor is disabled

/* PIC Configuration 2 */
__CONFIG(WRT_OFF &		// Flash Memory Self-Write Protection : OFF
		VCAPEN_OFF &	// VDDCORE pin functionality is disabled
		PLLEN_OFF &		// 4x PLL disabled
		STVREN_ON &		// Stack Overflow or Underflow will not cause a Reset
		BORV_HI &		// Brown-out Reset Voltage Selection : High Voltage
		DEBUG_OFF &		// In-Circuit Debugger disabled, ICSPCLK and ICSPDAT are general purpose I/O pins
		LVP_OFF);		// Low-voltage programming : disable


#ifndef _XTAL_FREQ
	/* 例：4MHzの場合、4000000 をセットする */
	#define _XTAL_FREQ 4000000
#endif

// 動作間隔（秒）。この間隔毎に測定値をLCD表示しEEPROMに記録する
#define WORK_INTERVAL   60
// EEPROMの容量 (24LC64=0x1fff, 24LC128=0x3fff, 24LC256=0x7fff）
#define EEPROM_MAX_SIZE 0x1fff

unsigned char flag_record_enable = 0;   // EEPROM記録のON/OFF
unsigned char flag_serial_enable = 1;   // シリアル出力のON/OFF

// RS232Cに改行を送信する
void print_crlf(void){
    if(flag_serial_enable) rs232c_puts("\r\n");
}

// LCDおよびRS232Cに文字列bufを出力（送信）する
void print_str(const char *buf){
	i2c_lcd_puts(buf);
	if(flag_serial_enable) rs232c_puts(buf);
}

// LCDおよびRS232Cに数値を文字列化して出力（送信）する
void print_uint(unsigned int n, unsigned char digit){
    unsigned char buf[3];

    // 2ケタの10進数（表示データがない倍は空白文字表示する）
    if(digit == 2){
        i2c_lcd_puts(uint_to_dec2(n, buf));
        if(flag_serial_enable) rs232c_puts(uint_to_dec2(n, buf));
    }
    // 1桁の10進数（表示データがない倍はゼロ表示する）
    else{
        i2c_lcd_puts(uint_to_dec1_0(n, buf));
        if(flag_serial_enable) rs232c_puts(uint_to_dec1_0(n, buf));
    }
}

// LCDにRS232CON/OFF,EEPROM書き込みON/OFF状態表示
void disp_info_to_lcd(void){
    // カーソル位置を1行目行頭へ
    i2c_lcd_set_cursor_pos(0);
    // シリアル出力の状態表示
    i2c_lcd_puts("Seri:O");
    if(flag_serial_enable) i2c_lcd_puts("N ");
    else i2c_lcd_puts("FF");
    // EEPROM記録の状態表示
    i2c_lcd_puts(" Mem:O");
    if(flag_record_enable) i2c_lcd_puts("N ");
    else i2c_lcd_puts("FF");
}

// データをEEPROMに追記する
void write_data_to_eeprom(unsigned int data){
    // 現在データのアドレス値をEEPROMアドレス0から読み込む
    unsigned int addr;

    i2c_enable();
    addr = (unsigned int)i2c_eeprom_read_byte(0) + ((unsigned int)i2c_eeprom_read_byte(1)<<8);
    // 2バイト進めて、今回のデータのアドレスとする
    addr += 2;
    if(addr < EEPROM_MAX_SIZE - 0x100){
        // データをEEPROMに書き込む
        i2c_eeprom_write_byte(addr, (unsigned char)(data & 0xff));
        i2c_eeprom_write_byte(addr+1, (unsigned char)(data >> 8));

        // 現在データのアドレス値をEEPROMアドレス0に書き込む
        i2c_eeprom_write_byte(0, (unsigned char)(addr & 0xff));
        i2c_eeprom_write_byte(1, (unsigned char)(addr >> 8));
    }
    i2c_disable();
}

// EEPROMの先頭2バイトに格納されたアドレスポインタに0を書き込む
void reset_address(void){
    i2c_enable();
    // アドレス値 0 をEEPROMアドレス0に書き込む
    i2c_eeprom_write_byte(0, (unsigned char)0);
    i2c_eeprom_write_byte(1, (unsigned char)0);
    i2c_disable();
}

// EEPROM内の全データをRS232Cに出力する
void dump_data(void){
    unsigned int addr, temperature;
    i2c_enable();
    // EEPROMから最大アドレス（2バイト）を読み出す
    addr = (unsigned int)i2c_eeprom_read_byte(0) | ((unsigned int)i2c_eeprom_read_byte(1)<<8);

    // アドレス2番地から、最大アドレスまで順に読みだして、RS232Cに送信（ダンプ）する
    for(unsigned int i=2; i<=addr; i+=2){
        rs232c_puts("d,");
        temperature = i2c_eeprom_read_byte(i) | (i2c_eeprom_read_byte(i+1) << 8);
        // 読みだしたデータを温度形式に換算する
        temperature = (unsigned int)((unsigned long int)temperature * 500L / 0x3ffL);
        // RS232Cに温度を10進数文字列化して送信する
        rs232c_puts(uint_to_dec2(temperature/10, rs232c_buffer));
        print_str(".");
        rs232c_puts(uint_to_dec1_0(temperature%10, rs232c_buffer));
        print_crlf();
    }

    i2c_disable();
}

int main(int argc, char** argv) {
    // 基本機能の設定
    OSCCON = 0b01101010;        // 内部オシレーター 4MHz
    TRISA = 0b00101111;         // IOポートRA0(AN0),RA1(SCL),RA2(SDA),RA5(RX)を入力モード（RA3は入力専用）、RA4(TX)を出力モード
    APFCONbits.RXDTSEL = 1;     // シリアルポート RXをRA5ピンに割付
    APFCONbits.TXCKSEL = 1;     // シリアルポート TXをRA4ピンに割付
    ANSELA = 0b00000001;        // A/D変換をAN0を有効、AN1,AN2,AN4を無効
    PORTA = 0;

    i2c_enable();
    OPTION_REGbits.nWPUEN = 0;  // I2C プルアップ抵抗 有効
    WPUA = 0b00000110;          // pull-up (RA1=SCL, RA2=SDA pull-up enable)

    ADCON0 = 0;                 // AN0選択, A/D機能停止
    ADCON1 = 0b10010000;        // 変換結果右詰, クロックFOSC/4, 比較対象VDD

    // 0.5秒待つ
    __delay_ms(500);

    i2c_lcd_init();
    rs232c_init(9);             // init 9600bps
    // RS-232C受信割り込みの有効化
    if(flag_serial_enable) rs232c_receive_interrupt_start();


    while(1){
        // A/D変換し、得られた値を温度表記に換算する
        ADCON0 = 0b00000001;            // AN0チャンネル選択, A/D機能ON
        __delay_us(10);                 // A/D変換器チャージ時間待つ
        ADCON0 = 0b00000011;            // AN0チャンネル選択, A/D開始, A/D機能ON
        while(ADCON0bits.GO_nDONE){}    // A/D変換完了を待つ
        unsigned int ad_value = ADRESH << 8 | ADRESL;
        ADCON0 = 0;                     // AN0選択, A/D機能停止
        unsigned int temperature = (unsigned int)((unsigned long int)ad_value * 500L / 0x3ffL);

        // LCD及びRS232Cに現在温度を出力（送信）する
        i2c_enable();
        i2c_lcd_clear();
        disp_info_to_lcd();
        i2c_lcd_set_cursor_pos(0x40);
        print_uint(temperature/10, 2);
        print_str(".");
        print_uint(temperature%10, 1);
        print_crlf();
        i2c_disable();

        // EEPROMにデータを記録する
        if(flag_record_enable) write_data_to_eeprom(ad_value);


        // WORK_INTERVAL 秒間待つ間に、RS232Cからのコマンド受付
        for(char i=0; i<WORK_INTERVAL; i++){
            __delay_ms(1000);   // 1秒スリープ
            // RS232Cで改行を伴う文字列入力を検知した場合
            if(flag_rs232c_received)
            {
                rs232c_receive_interrupt_stop();
                // EEPROMアドレスのリセット
                if(!strcmp(rs232c_buffer, "reset")){
                    reset_address();
                    print_crlf();
                }
                // EEPROMデータをRS232Cにダンプする
                else if(!strcmp(rs232c_buffer, "dump")){
                    dump_data();
                }
                // RS232Cでの受信待機モードに切り替える
                if(flag_serial_enable) rs232c_receive_interrupt_start();
            }

            // RA3ボタンが押された場合、RS232CのON/OFF，EEPROM書き込みのON/OFFモードを切り替える
			if(!PORTAbits.RA3)
			{
                if(flag_record_enable && flag_serial_enable) flag_record_enable = 0;
                else if(!flag_record_enable && flag_serial_enable) flag_serial_enable = 0;
                else if(!flag_record_enable && !flag_serial_enable) flag_record_enable = 1;
                else flag_serial_enable = 1;
                // RS232Cでの受信待機モードに切り替える
                if(flag_serial_enable) rs232c_receive_interrupt_start();
                else rs232c_receive_interrupt_stop();
                // 現在のモードをLCDに表示する
                i2c_enable();
                disp_info_to_lcd();
                i2c_disable();
			}
        }

    }

    return (EXIT_SUCCESS);
}

