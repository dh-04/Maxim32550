/*
 * @file security_monitor/include/extscn.h
 *
 ******************************************************************************
 * Copyright (C) 2016-2018 Maxim Integrated Products, Inc., All rights Reserved.
 *
 * This software is protected by copyright laws of the United States and
 * of foreign countries. This material may also be protected by patent laws
 * and technology transfer regulations of the United States and of foreign
 * countries. This software is furnished under a license agreement and/or a
 * nondisclosure agreement and may only be used or reproduced in accordance
 * with the terms of those agreements. Dissemination of this information to
 * any party or parties not specified in the license agreement and/or
 * nondisclosure agreement is expressly prohibited.
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, mask works rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 ******************************************************************************
 */

#ifndef EXTSCN_H_
#define EXTSCN_H_

#define IDLE		0
#define ARMED		1
#define TRIGGERED	2

/* AES */
#define AES_BASE 		0x40005000
#define AES_AESC_C		AES_BASE + 0x0
#define AESUKEYR0 		AES_BASE + 0x24
#define AESUKEYR1 		AES_BASE + 0x28
#define AESUKEYR2 		AES_BASE + 0x2C
#define AESUKEYR3 		AES_BASE + 0x30
#define AESDIN0 		AES_BASE + 0x04
#define AESDIN1 		AES_BASE + 0x08
#define AESDIN2 		AES_BASE + 0x0C
#define AESDIN3 		AES_BASE + 0x10
#define AESDOUT0 		AES_BASE + 0x14
#define AESDOUT1 		AES_BASE + 0x18
#define AESDOUT2 		AES_BASE + 0x1C
#define AESDOUT3 		AES_BASE + 0x20

#define START_UAES_ENC	0x101  // Start AES encryption by using 128-bit user key
#define START_SAES_ENC	0x01  // Start AES encryption by using 128-bit secure key

#define NUMBER_OF_EXT_SENS	6  // Number of external sensors
/*  Mask value for external sensors
 *  if ext sensors number is 6 mask value will be 0x3f
 *  if ext sensors number is 4 mask value will be 0xf
 */
#define EXT_SENS_MASK		((1<<NUMBER_OF_EXT_SENS)-1)

int set_sensors(char internal, unsigned int int_conf, char external,
		unsigned int ext_conf, unsigned char *status);
void get_aes_result(unsigned int *data);
void keywipe_test(void);
int external_sens_tests(unsigned char *status, unsigned char *previous,
		int trace);
void ack_drs(void);
int check_aes_key(void);
int load_aes_key(void);
void reload_system_after_drs(void);
void wfa(void);

#endif /** EXTSCN_H_ */

/******************************************************************************/
/* EOF */
