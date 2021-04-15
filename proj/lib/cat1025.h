#ifndef _CAT1025_H_
#define _CAT1025_H_

/* CAT1025 
 *   I2C serial 2 kbit CMOS EEPROM (256 byte)
 *   address : 1 byte
 */

/* cat1025_read
 *   read multiple bytes from EEPROM beginning at 'address'
 */
int cat1025_read(uint32_t address, uint8_t *data, uint32_t nb);

/* cat1025_write
 *   can write up to 16 bytes of data to EEPROM beginning at 'address'
 */
int cat1025_write(uint32_t address, uint8_t *data, uint32_t nb);


#endif
