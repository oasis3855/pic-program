/*
 * DHT11（温湿度センサー）の読み出しライブラリ
 *
 * GNU GPL Free Software (C) INOUE Hirokazu
 *
 */

#ifndef DHT11_H
#define	DHT11_H

#ifdef	__cplusplus
extern "C" {
#endif

void dht11_init_port(void);
unsigned char dht11_read_value(unsigned char *value);
unsigned char dht11_read_byte(void);

#ifdef	__cplusplus
}
#endif

#endif	/* DHT11_H */

