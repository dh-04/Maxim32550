/*
 * ccitt_crc.h
 *
 *  Created on: Dec 15, 2016
 *      Author: vietdang
 */

#ifndef CCITT_CRC_INC_CCITT_CRC_H_
#define CCITT_CRC_INC_CCITT_CRC_H_
/*
//                                16  12  5
// The CCITT CRC 16 polynomial is X + X + X + 1.
// In binary, this is the bit pattern 1 0001 0000 0010 0001, and in hex it
//  is 0x11021.
// A 17 bit register is simulated by testing the MSB before shifting
//  the data, which affords us the luxury of specifiy the polynomial as a
//  16 bit value, 0x1021.
// Due to the way in which we process the CRC, the bits of the polynomial
//  are stored in reverse order. This makes the polynomial 0x8408.
*/
#define POLY 0x8408
//#define POLY 0x11021
unsigned short ccitt_crc(unsigned char *data_p, unsigned short length);

#endif /* CCITT_CRC_INC_CCITT_CRC_H_ */
