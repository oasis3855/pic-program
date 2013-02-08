/*
 * File:   i2c-eepromlib.c   for Microchip PIC XC8 (12F1822, 16F1823, 16F1827)
 *
 * Microchip I2C EEPROM 24LC32, 24LC64, 24LC128, 24LC256, 24LC512用
 */

#include <stdio.h>
#include <stdlib.h>

#include <xc.h>
#include "i2c-lib.h"

#ifndef _XTAL_FREQ
    /* 例：4MHzの場合、4000000 をセットする */
    #define _XTAL_FREQ 4000000
#endif


#define EEPROM_I2C_ADDR 0x51


// EEPROMに1バイト書き込む
void i2c_eeprom_write_byte(unsigned int addr, unsigned char data){
    i2c_start();
    i2c_send_byte(EEPROM_I2C_ADDR << 1);    // i2cアドレスを1ビット左にシフトし、末尾にR/Wビット（Write=0）を付与
    i2c_send_byte((unsigned char)(addr>>8));        // アドレスの上位バイト
    i2c_send_byte((unsigned char)(addr&0xff));      // アドレスの下位バイト
    i2c_send_byte(data);
    i2c_stop();

    __delay_ms(10);     // Wait Twc = 5 ms

    return;
}

// EEPROMに1ページ分（32バイト）書き込む
void i2c_eeprom_write_32byte(unsigned int addr, unsigned char *data){
    i2c_start();
    i2c_send_byte(EEPROM_I2C_ADDR << 1);    // i2cアドレスを1ビット左にシフトし、末尾にR/Wビット（Write=0）を付与
    i2c_send_byte((unsigned char)(addr>>8));        // アドレスの上位バイト
    i2c_send_byte((unsigned char)(addr&0xff));      // アドレスの下位バイト
    for(int i=0; i<32; i++) i2c_send_byte(data[i]);
    i2c_stop();

    __delay_ms(20);     // Wait Twc + Tload = 5 + 14 ms (from AN1028 'Recommended Usage of Microchip I2C EEPROM' page 5)

    return;
}

// EEPROMから1バイト読み出す
unsigned char i2c_eeprom_read_byte(unsigned int addr){
    i2c_start();
    i2c_send_byte(EEPROM_I2C_ADDR<<1);  // i2cアドレスを1ビット左にシフトし、末尾にR/Wビット（Write=0）を付与
    i2c_send_byte((unsigned char)(addr>>8));        // アドレスの上位バイト
    i2c_send_byte((unsigned char)(addr&0xff));      // アドレスの下位バイト
    i2c_repeat_start();
    i2c_send_byte((EEPROM_I2C_ADDR<<1) | 1);    // i2cアドレスを1ビット左にシフトし、末尾にR/Wビット（Read=1）を付与
    unsigned char data = i2c_read_byte(0);  // 読み込み後NO_ACK
    i2c_stop();

    __delay_us(30);

    return(data);
}

// EEPROMから1ページ分（32バイト）読み出す
void i2c_eeprom_read_32byte(unsigned int addr, unsigned char *buf){
    i2c_start();
    i2c_send_byte(EEPROM_I2C_ADDR<<1);  // i2cアドレスを1ビット左にシフトし、末尾にR/Wビット（Write=0）を付与
    i2c_send_byte((unsigned char)(addr>>8));        // アドレスの上位バイト
    i2c_send_byte((unsigned char)(addr&0xff));      // アドレスの下位バイト
    i2c_repeat_start();
    i2c_send_byte((EEPROM_I2C_ADDR<<1) | 1);    // i2cアドレスを1ビット左にシフトし、末尾にR/Wビット（Read=1）を付与
    for(int i=0; i<31; i++) buf[i] = i2c_read_byte(1);  // 読み込み後ACK
    buf[31] = i2c_read_byte(0); // 読み込み後NO_ACK
    i2c_stop();

    __delay_us(30);

    return;
}

