/* 
 * File:   serial-lib.h
 */

#ifndef SERIAL_LIB_H
#define	SERIAL_LIB_H

#ifdef	__cplusplus
extern "C" {
#endif

    void rs232c_send_char_delay(unsigned char ch);
void rs232c_send_char_timer(unsigned char ch);
void rs232c_putch(const char c);
void rs232c_puts(const char * s);

void rs232c_test_pattern(void);

#ifdef	__cplusplus
}
#endif

#endif	/* SERIAL_LIB_H */

