/* 
 * File:   common-lib.h
 * Author: vm
 *
 * Created on 2013/02/03, 16:41
 */

#ifndef COMMON_LIB_H
#define	COMMON_LIB_H

#ifdef	__cplusplus
extern "C" {
#endif

unsigned char *uchar_to_hex2(unsigned char c, unsigned char *buf);
unsigned char *uint_to_hex4(unsigned int c, unsigned char *buf);
unsigned char *uchar_to_bin8(unsigned char c, unsigned char *buf);
unsigned char *uint_to_dec5(unsigned int c, unsigned char *buf);
unsigned char *uint_to_dec2(unsigned int c, unsigned char *buf);
unsigned char *uint_to_dec1_0(unsigned int c, unsigned char *buf);



#ifdef	__cplusplus
}
#endif

#endif	/* COMMON_LIB_H */

