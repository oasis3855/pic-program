/* 
 * File:   Lib-rs232c.h
 * Author: vm
 *
 * Created on 2012/08/26, 9:32
 */

#ifndef LIB_RS232C_H
#define	LIB_RS232C_H

#ifdef	__cplusplus
extern "C" {
#endif

// extern int flag_rs232c_received;    // ■■ 受信機能を使わない場合はコメントアウト
// extern char rs232c_buffer[];	    // ■■ 受信機能を使わない場合はコメントアウト

void rs232c_init(void);
void rs232c_putch(char c);
void rs232c_puts(const char * s);

#ifdef	__cplusplus
}
#endif

#endif	/* LIB_RS232C_H */

