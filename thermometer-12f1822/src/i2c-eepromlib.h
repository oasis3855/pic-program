/* 
 * File:   i2c-eepromlib.h   for Microchip PIC XC8
 */

#ifndef I2C_EEPROMLIB_H
#define	I2C_EEPROMLIB_H

#ifdef	__cplusplus
extern "C" {
#endif

void i2c_eeprom_write_byte(unsigned int addr, unsigned char data);
void i2c_eeprom_write_32byte(unsigned int addr, unsigned char *data);
unsigned char i2c_eeprom_read_byte(unsigned int addr);
void i2c_eeprom_read_32byte(unsigned int addr, unsigned char *buf);


#ifdef	__cplusplus
}
#endif

#endif	/* I2C_EEPROMLIB_H */

