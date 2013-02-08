#include <stdio.h>

#ifndef _XTAL_FREQ
	/* 例：4MHzの場合、4000000 をセットする */
	#define _XTAL_FREQ 4000000
#endif

// 16進数変換が必要ない場合は、HEX_Dataがメモリ上に確保されるのを防ぐため、コメントアウトすること
#if 0

char HEX_Data[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'} ;

// unsigned char を2ケタの16進数の文字列に変換する（右詰め、ゼロ埋めあり）
// bufは3バイト必要
unsigned char *uchar_to_hex2(unsigned char c, unsigned char *buf)
{
//    char HEX_Data[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'} ;

    buf[0] = HEX_Data[ (c >> 4) ];  // 2桁目
    buf[1] = HEX_Data[ c & 0x0f ];  // 1桁目
    buf[2] = (unsigned char)0;
    return(buf);
}

// unsigned int を4ケタの16進数の文字列に変換する（右詰め、ゼロ埋めあり）
// bufは5バイト必要
unsigned char *uint_to_hex4(unsigned int c, unsigned char *buf)
{
//    char HEX_Data[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'} ;

    buf[0] = HEX_Data[ (c >> 12) ];        // 4桁目
    buf[1] = HEX_Data[ (c >> 8) & 0x0f ];  // 3桁目
    buf[2] = HEX_Data[ (c >> 4) & 0x0f ];  // 2桁目
    buf[3] = HEX_Data[ c & 0x0f ];          // 1桁目
    buf[4] = (unsigned char)0;
    return(buf);
}

#endif

// unsigned char を8ケタの2進数の文字列に変換する（右詰め、ゼロ埋めあり）
// bufは9バイト必要
unsigned char *uchar_to_bin8(unsigned char c, unsigned char *buf)
{
    signed char i;
    for (i=7 ; i>=0 ; i--) {
        if ( ((c >> i) & 0x1) == 1) buf[7-i] = '1';
        else                         buf[7-i] = '0';
    }
    buf[8] = (unsigned char)0;

    return(buf);
}

// unsigned int を5ケタの10進数の文字列に変換する（右詰め、空白埋めあり）
// bufは6バイト必要
unsigned char *uint_to_dec5(unsigned int c, unsigned char *buf)
{
    signed char i;
    for(i=0; i<=5; i++) buf[i] = ' ';
    i = 6;
    do {
        i-- ;
        buf[i] = (char)((c%10) + 0x30) ;
        c = c / 10 ;
    } while(c != 0) ;
    buf[6] = (unsigned char)0;


    return(buf);
}

// unsigned int を2ケタの10進数の文字列に変換する（右詰め、空白埋めあり）
// bufは3バイト必要
unsigned char *uint_to_dec2(unsigned int c, unsigned char *buf)
{
    signed char i;
    for(i=0; i<2; i++) buf[i] = ' ';
    i = 2;
    do {
        i-- ;
        buf[i] = (char)((c%10) + 0x30) ;
        c = c / 10 ;
    } while(c != 0) ;
    buf[2] = (unsigned char)0;


    return(buf);
}

// unsigned int を1ケタの10進数の文字列に変換する（右詰め、0埋めあり）
// bufは3バイト必要
unsigned char *uint_to_dec1_0(unsigned int c, unsigned char *buf)
{
    buf[0] = (char)((c%10) + 0x30) ;
    buf[1] = (unsigned char)0;
    return(buf);
}
