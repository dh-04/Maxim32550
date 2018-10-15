/*
 * @file i2c/src/main.c
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
 * @defgroup EX_I2C I2C Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example show you how to configure I2C0 to send and read data from a slave.
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include <MAX325xx.h>


#define EX_I2C_VERS_MAJOR	<VERSMAJ>
#define EX_I2C_VERS_MINOR	<VERSMIN>
#define EX_I2C_VERS_PATCH	<VERSPAT>
#define EX_I2C_VERSION_STRING	"v" xstr(EX_I2C_VERS_MAJOR) "." xstr(EX_I2C_VERS_MINOR) "." xstr(EX_I2C_VERS_PATCH)


mml_i2c_dev_t devnum = MML_I2C_DEV0; /* I2C port to configure */
unsigned short slave_address = 0xB0; /* I2C 7bits slave address is 0xB0 = 0b1011 0000. The last 0 is for the read/write bit.
 	 	 	 	 	 	 	 	 	  * The real 7bits slave address is 0x58 = 0b101 1000
 	 	 	 	 	 	 	 	 	  */

int init_uart(void);

int i2c_read(unsigned char *register_adress, unsigned char *data, unsigned int *plength);

int i2c_write(unsigned char *data, unsigned int *plength);

int init_uart(void) {
	int result = COMMON_ERR_UNKNOWN;
	mml_uart_config_t config;

	config.baudrate = 115200;
	config.data_bits = UARTn_CTRL_SIZE_bits8;
	config.flwctrl = UARTn_CTRL_RTSCTSF_disable;
	config.parity = MML_UART_PARITY_NONE;
	config.parity_mode = MML_UART_PARITY_MODE_ONES;
	config.rts_ctl = UARTn_PIN_RTS_lo;
	config.stop_bits = UARTn_CTRL_STOP_stop1;
	result = mml_uart_init(MML_UART_DEV0, config);
	return result;
}

int i2c_read(unsigned char *register_adress, unsigned char *data, unsigned int *plength) {
	int result = COMMON_ERR_UNKNOWN;
	unsigned int i;
	unsigned int len = 1;
	unsigned int length_requested = *plength;

	/* Command to select a slave */
	result = mml_i2c_write_start(devnum, slave_address, register_adress, &len);
	if (result) {
		printf("Error in mml_i2c_write_start function\n");
		return result;
	}
	/* Command to read data from slave selected in write_start */
	result = mml_i2c_read_start(devnum, slave_address);
	if (result) {
		printf("Error in mml_i2c_read_start function\n");
		return result;
	}
	len = 0;
	do {
		result = mml_i2c_read_attended(devnum, data + (length_requested - *plength), *plength, &len);
		if (result) {
			printf("error in mml_i2c_read");
		}
		*plength -= len;
	} while(*plength);
	printf("First bytes transmitted\n");
	for (i=0; i<length_requested; i++) {
		printf("data_receive[%d] : 0x%08x\n",i,*(data+i));
	}
	return result;
}

int i2c_write(unsigned char *data, unsigned int *plength) {
	int result = COMMON_ERR_UNKNOWN;
	unsigned int length_requested = *plength;
	/* Command to start write with a slave specified by slave_adress */
	result = mml_i2c_write_start(devnum, slave_address, data, plength);
	if (result)
		return result;
	do {
		result = mml_i2c_write_attended(devnum, data, length_requested, plength);
		if (result) {
			printf("Error in mml_i2c_write\n");
			return result;
		}
		data += *plength;
		length_requested -= *plength;
	} while(length_requested);
	if (result)
		return result;
	return result;
}

int main(void) {
	int result = COMMON_ERR_UNKNOWN;
	unsigned int i = 0;
	unsigned int	length_receive			  = 15; /* Number of byte to receive */
	unsigned int	length_transmit			  = 15; /* Number of byte to transmit */
	unsigned char	register_address_to_read  = 0x20; /* Register from data will be read */
	unsigned char	data_receive[15]		  = {0x00, 0x00, 0x00, 0x00, 0x00,
												 0x00, 0x00, 0x00, 0x00, 0x00,
												 0x00, 0x00, 0x00, 0x00, 0x00}; /* Variable where data read will be stored */
	unsigned char	data_transmit[15]		  = {0x20, 0x21, 0x22, 0x23, 0x24,
												 0x25, 0x26, 0x27, 0x28, 0x29,
												 0x30, 0x31, 0x32, 0x33, 0x34}; /* Data to write */
	mml_i2c_config_t i2c_config;

	/* Initialize UART0 port with default configurations */
	result = init_uart();
	if (result)
		return result;
	printf("\n\nWelcome to I2C application\n");
	/* Configure I2C */
	i2c_config.baudrate = 100000; /* Configure a baudrate of 100kHz */
	i2c_config.rxfifo_thr = I2C_CR_RXTHD_word8;
	i2c_config.flags = 0; /* If you want to configure auto start on I2C puts 0x0010 in i2c_config.flags;*/
	i2c_config.irq_handler = NULL;
	/* Initialize I2C device */
	result = mml_i2c_init(devnum, &i2c_config);
	if (result) {
		printf("Error at I2C initialization \n");
		return result;
	}
	/* WRITE OPERATION */
	printf("\n****	WRITE OPERATION	****\n");
	for (i=0; i<length_transmit; i++) {
		printf("Byte to transmit is 0x%08x:\n", (unsigned int)data_transmit[i]);
	}
	result = i2c_write(data_transmit, &length_transmit);
	if (result) {
		printf("Error in write operation\n");
		return result;
	}
	printf("Write operation complete\n");
	/* READ OPERATION */
	printf("\n****	READ OPERATION	****\n");
	printf("Read data from slave at address 0x%08x\n", slave_address);
	printf("Read data from register 0x%08x\n", register_address_to_read);
	/* Read data from a slave */
	result = i2c_read(&register_address_to_read, data_receive, &length_receive);
	if (result) {
		printf("Error in read function\n");
		return result;
	}
	printf("Read operation complete\n");
	return result;
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__

#pragma GCC diagnostic pop

