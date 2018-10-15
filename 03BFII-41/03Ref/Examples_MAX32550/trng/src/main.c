/*
 * @file trng/src/main.c
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
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 ******************************************************************************
 */

/**
 * @defgroup EX_TRNG TRNG Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example show you how to generate 3 AES Key with TRNG.
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

/* Global includes */
#include <MAX325xx.h>
#include <stdio.h>
#include <string.h>
#include <errors.h>
/* Local includes */
#include <trng.h>


#define EX_TRNG_VERS_MAJOR	<VERSMAJ>
#define EX_TRNG_VERS_MINOR	<VERSMIN>
#define EX_TRNG_VERS_PATCH	<VERSPAT>
#define EX_TRNG_VERSION_STRING	"v" xstr(EX_TRNG_VERS_MAJOR) "." xstr(EX_TRNG_VERS_MINOR) "." xstr(EX_TRNG_VERS_PATCH)


int main(void) {
	unsigned char testOk = 1;
	int result = COMMON_ERR_UNKNOWN;
	unsigned int currentKeyIndex = 0;
	unsigned int followingKeyIndex = 1;
	unsigned int index = 0;
	unsigned char AESKeys[K_AES_KEYS_NB][K_COBRA_USECASE_TRNG_AES_KEY_SIZE];
	mml_uart_config_t uart_conf;

	/* Initialize UATR0 port with default configurations */
	uart_conf.baudrate = 115200;
	uart_conf.data_bits = UARTn_CTRL_SIZE_bits8;
	uart_conf.flwctrl = UARTn_CTRL_RTSCTSF_disable;
	uart_conf.parity = MML_UART_PARITY_NONE;
	uart_conf.parity_mode = MML_UART_PARITY_MODE_ONES;
	uart_conf.rts_ctl = UARTn_PIN_RTS_lo;
	uart_conf.stop_bits = UARTn_CTRL_STOP_stop1;
	result = mml_uart_init(MML_UART_DEV0, uart_conf);
	if (result) {
		return result;
	}
	/* Welcome message */
	printf("Welcome to TRNG usecase application !\n");
	/* AES key initialization  */
	memset(AESKeys, 0x00, ( K_COBRA_USECASE_TRNG_AES_KEY_SIZE * K_AES_KEYS_NB));
	/* Generate AES keys */
	for (index = 0; index < K_AES_KEYS_NB; index++) {
		unsigned char *Key = &AESKeys[index][0];

		usecase_trng_gen_aes_key(Key,
		K_COBRA_USECASE_TRNG_AES_KEY_SIZE);
	}
	/* Display generated AES keys */
	for (index = 0; index < K_AES_KEYS_NB; index++) {
		unsigned char *Key = &AESKeys[index][0];
		unsigned int index_1;

		printf("\nAES KEY %d:\n", index);
		for (index_1=0; index_1 < K_COBRA_USECASE_TRNG_AES_KEY_SIZE; index_1++) {
			printf("%02X", Key[index_1]);
		}
	}
	while ((currentKeyIndex < ( K_AES_KEYS_NB - 1)) && testOk) {
		unsigned char *Key_1 = &AESKeys[currentKeyIndex][0];
		unsigned char *Key_2 = &AESKeys[followingKeyIndex][0];

		if (0 == memcmp(Key_1, Key_2,
		K_COBRA_USECASE_TRNG_AES_KEY_SIZE)) {
			printf("\nAES KEY %d is identical to AES KEY %d !!!\n",
					currentKeyIndex, followingKeyIndex);
			testOk = 0;
			continue;
		}
		if (( K_AES_KEYS_NB - 1) == followingKeyIndex) {
			currentKeyIndex++;
			followingKeyIndex = currentKeyIndex + 1;
		} else
			followingKeyIndex++;
	}
	printf("\n");
	if (testOk) {
		printf("\n\nTRNG usecase passed OK.\n");
	} else
		printf("\n\nTRNG usecase passed NOK.\n");

	return result;
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__

/******************************************************************************/
/* EOF */
