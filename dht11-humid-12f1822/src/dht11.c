/*
 * DHT11（温湿度センサー）の読み出しライブラリ
 *
 * GNU GPL Free Software (C) INOUE Hirokazu
 *
 */

#include <stdio.h>
#include <xc.h>
#include "dht11.h"

#ifndef _XTAL_FREQ
    /* 例：4MHzの場合、4000000 をセットする */
    #define _XTAL_FREQ 4000000
#endif

/*
 * DHT11のI/Oピンが接続されるPICのポート
 */
#define PORT_DHT11      PORTAbits.RA0
#define TRIS_DHT11      TRISAbits.TRISA0
#define WPUA_DHT11      WPUAbits.WPUA0

void dht11_init_port(void) {
    // DHT11のI/Oバスは、VCCでプルアップする必要があるため、PICのプルアップ抵抗をONにする
    // （または、ハードウエア的に 5.1kΩ程度の抵抗を用いる方法でもよい）
    WPUA_DHT11 = 1;
    // 「DHT11接続ポート」を入力に設定
    TRIS_DHT11 = 1;
    // DHT11に電源投入後 1秒待つ
    __delay_ms(1000);
}

unsigned char dht11_read_value(unsigned char *value) {
    // DHT11に処理開始信号を送る (Loを18ms以上続ける）
    // I/O = Lo
    TRIS_DHT11 = 0; // 「DHT11接続ポート」を出力モードに
    PORT_DHT11 = 0; // 「DHT11接続ポート」 = Lo
    // 25 msec Loを保持（18 msec以上）
    __delay_ms(25);
    // I/O = Hi （入力モードにすることで、自動的にプルアップされる）
    TRIS_DHT11 = 1; // 「DHT11接続ポート」を入力モードに
    // プルアップ抵抗で「DHT11接続ポート」が充電される時間待つ（実測では5〜10usec以上）
    __delay_us(20);

    // DHT11からの開始確認信号を受信する（Lo 80usec, Hi 80usec）
    // 「DHT11接続ポート」 = Lo になるまで待つ
    unsigned char k = 150;    // 300 usec以上待っても終了しない場合のカウンタ
    while(!PORT_DHT11) {
        __delay_us(2);
        if(--k < 1) return(0);  // 異常終了
    }
    // 「DHT11接続ポート」 = Hi になるまで待つ
    k = 150;    // 300 usec以上待っても終了しない場合のカウンタ
    while(PORT_DHT11) {
        __delay_us(2);
        if(--k < 1) return(0);  // 異常終了
    }

    // データの読み出し（5バイト）
    *value = dht11_read_byte();         // 湿度 整数部
    *(value+1) = dht11_read_byte();     // 湿度 小数部（DHT11ではゼロ）
    *(value+2) = dht11_read_byte();     // 温度 整数部
    *(value+3) = dht11_read_byte();     // 温度 小数部（DHT11ではゼロ）
    *(value+4) = dht11_read_byte();     // チェックサム

// ループで読み出しを行うと、タイミングがずれて読み取れない
//    for (short int i = 0; i < 4; i++) {
//        *(value + i) = dht11_read_byte();
//    }

    // チェックサムの確認
    if(*value + *(value+1) + *(value+2) + *(value+3) != *(value+4)) return(0);

    // チェックサム結果OK
    return(1);
}

unsigned char dht11_read_byte(void) {
      static unsigned short num, i;
      num = 0;
      for (i=0; i<8; i++){
           while(!PORT_DHT11);
           __delay_us(40);  // データが0の場合26-28usec, 1の場合70usecの中間あたりの秒数
           if(PORT_DHT11) num |= 1<<(7-i);
           while(PORT_DHT11);
       }
      return num;
}
