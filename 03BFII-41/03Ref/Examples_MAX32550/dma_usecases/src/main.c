/*
 * @file dma_usecases/src/main.c
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
 * @defgroup EX_DMA DMA Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example show you how to configure DMA in 3 different transfer modes.
 *
 *	1- memory to memory transfer:
 *		-this test copy data from buffer[0][0] to buffer[1][0]
 *	2- memory to memory chain transfer:
 *		-this test copy data:
 *			- from buffer[2][0] to buffer[3][0]
 *			- from buffer[4][0] to buffer[5][0]
 *			- from buffer[6][0] to buffer[7][0]
 *			- from buffer[8][0] to buffer[9][0]
 *   3- memory to memory with new configuration
 *		-this test copy data from buffer[0][0] to buffer[1][0]
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

#include <MAX325xx.h>
#include <string.h>
#include <stdio.h>
#include <dma.h>

#define EX_DMA_VERS_MAJOR	<VERSMAJ>
#define EX_DMA_VERS_MINOR	<VERSMIN>
#define EX_DMA_VERS_PATCH	<VERSPAT>
#define EX_DMA_VERSION_STRING	"v" xstr(EX_DMA_VERS_MAJOR) "." xstr(EX_DMA_VERS_MINOR) "." xstr(EX_DMA_VERS_PATCH)


/* Memory buffers for DMA transfers */
char buffer[10][BUFFER_TRANSFER_SIZE];
static volatile int chain_transfer = 0;
/* DMA channel */
mml_dma_channel_t channel;

static void realod_callback(mml_dma_addr_t *rld_addr, int *reload);

int dma_tests(void) {
	int result = COMMON_ERR_UNKNOWN;
	/* DMA configuration info */
	mml_dma_config_t config;
	/* DMA address */
	mml_dma_addr_t addr;

	/* Initialize the memory buffer to zero */
	memset((char*) &buffer, 0, sizeof(buffer));
	config.priority = DMAn_CFG_PRI_low;
	config.request = DMAn_CFG_REQSEL_memToMem;
	config.req_wait = MML_DMA_START_TIMER_NORMAL;
	config.timeout = DMAn_CFG_TOSEL_to4;
	config.prescale = DMAn_CFG_PSSEL_disable;
	config.src_width = DMAn_CFG_SRCWD_word;
	config.src_increment = 1;
	config.dst_width = DMAn_CFG_DSTWD_word;
	config.dst_increment = 1;
	config.burst_size = 32;
	config.handler = NULL;
	addr.src = (unsigned int) &buffer[0][0];
	addr.dst = (unsigned int) &buffer[1][0];
	addr.count = BUFFER_TRANSFER_SIZE;
	/* Reset DMA interface */
	GCR->RSTR |= GCR_RSTR_DMA_Msk;
	/* Wait until DMA reset completes */
	while ( GCR_RSTR_DMA_Msk & GCR->RSTR);
	/* Get free channel DMA */
	channel = mml_dma_get_free_channel();
	/* Initialize DMA channel */
	result = mml_dma_init_without_irq(channel, config, addr);
	if (result) {
		/* Failed to initialize DMA channel */
		return result;
	}
	printf("DMA configuration passed\n");

	/* Call usecase test #1 */
	result = dma_mem2mem_transfer_test();
	if (result) {
		/* Use case failed */
		return result;
	}
	/* Call usecase test #2 */
	result = dma_mem2mem_chaintransfer_test();
	if (result) {
		/* Use case failed */
		return result;
	}
	/* Call usecase test #3 */
	result = dma_mem2mem_newconfiguration_test();
	return result;
}

int dma_mem2mem_transfer_test(void) {
	int result = COMMON_ERR_UNKNOWN;
	mml_dma_status_t status;

	/* Initialize the source memory buffer address with the know data */
	memset(&buffer[0][0], 0xAB, BUFFER_TRANSFER_SIZE);
	/* Zero initialize the destination memory buffer */
	memset(&buffer[1][0], 0x00, BUFFER_TRANSFER_SIZE);

	printf("\n\nTest of dma_mem2mem_transfer_test:\n");
	printf("This test copy data of buffer[0][0] to buffer[1][0]\n");
	printf("buffer[0][0]: %d \n", buffer[0][0]);
	printf("buffer[1][0]: %d \n", buffer[1][0]);
	/* Enable the DMA channel transfer */
	result = mml_dma_enable(channel);
	if (result) {
		printf("DMA Usecase Error: dma_mem2mem_transfer_test -> mml_dma_enable()\n\r");
		return result;
	}
	/* Wait until DMA channel transfer completes */
	do {
		result = mml_dma_get_status(channel, &status);
		if (result) {
			printf("DMA Usecase Error: dma_mem2mem_transfer_test -> mml_dma_get_status()\n\r");
			return result;
		}
	} while (MML_DMA_STATUS_ENABLED == status);
	if (memcmp(&buffer[0][0], &buffer[1][0], BUFFER_TRANSFER_SIZE)) {
		result = COMMON_ERR_NO_MATCH;
	} else {
		printf("\ndma_mem2mem_transfer_test passed !\n");
		printf("New value of buffer[0][0]: %d\n", buffer[0][0]);
		printf("New value of buffer[1][0]: %d\n", buffer[1][0]);
		result = NO_ERROR;
	}
	return result;
}

int dma_mem2mem_chaintransfer_test(void) {
	int result = COMMON_ERR_UNKNOWN;
	/* DMA address */
	mml_dma_addr_t addr;
	mml_dma_status_t status;

	/* Fill the memory buffers with known data */
	memset(&buffer[2][0], 0xA5, BUFFER_TRANSFER_SIZE);
	memset(&buffer[4][0], 0x5A, BUFFER_TRANSFER_SIZE);
	memset(&buffer[6][0], 0xDE, BUFFER_TRANSFER_SIZE);
	memset(&buffer[8][0], 0xAD, BUFFER_TRANSFER_SIZE);

	printf("\n\nTest of dma_mem2mem_chaintransfer_test:\n");
	printf("This test copy data of buffer[2][0] to buffer[3][0], buffer[4][0] to buffer[5][0], buffer[6][0] to buffer[7][0], buffer[8][0] to buffer[9][0]\n");
	printf("buffer[2][0]: %d \n", buffer[2][0]);
	printf("buffer[3][0]: %d \n", buffer[3][0]);
	printf("buffer[4][0]: %d \n", buffer[4][0]);
	printf("buffer[5][0]: %d \n", buffer[5][0]);
	printf("buffer[6][0]: %d \n", buffer[6][0]);
	printf("buffer[7][0]: %d \n", buffer[7][0]);
	printf("buffer[8][0]: %d \n", buffer[8][0]);
	printf("buffer[9][0]: %d \n", buffer[9][0]);

	/* Set the new DMA address */
	addr.src = (unsigned int) &buffer[2][0];
	addr.dst = (unsigned int) &buffer[3][0];
	addr.count = BUFFER_TRANSFER_SIZE;
	result = mml_dma_set_address(channel, addr);
	DMAC->CN |= ( MML_DMA_CN_CHIEN_ENABLE << channel);
	if (result) {
		printf("DMA Usecase Error: dma_mem2mem_chaintransfer_test -> mml_dma_set_address()\n\r");
		return result;
	}
	/* Configure the chain transfer */
	addr.src = (unsigned int) &buffer[4][0];
	addr.dst = (unsigned int) &buffer[5][0];
	addr.count = BUFFER_TRANSFER_SIZE;
	result = mml_dma_set_chaintransfer(channel, addr, realod_callback);
	if (result) {
		printf("DMA Usecase Error: dma_mem2mem_chaintransfer_test -> mml_dma_set_chaintransfer()\n\r");
		return result;
	}
	/* Enable the DMA channel transfer */
	result = mml_dma_enable(channel);

	if (result) {
		printf("DMA Usecase Error: dma_mem2mem_chaintransfer_test -> mml_dma_enable()\n\r");
		return result;
	}
	/* Wait till last buffer chain transfer completes */
	do {
		result = mml_dma_get_status(channel, &status);
		if (result) {
			printf("DMA Usecase Error: dma_mem2mem_chaintransfer_test -> mml_dma_get_status()\n\r");
			return result;
		}
	} while (!chain_transfer && (MML_DMA_STATUS_ENABLED == status));
	/* Compare the buffers */
	if (memcmp(&buffer[2][0], &buffer[3][0], BUFFER_TRANSFER_SIZE)) {
		result = COMMON_ERR_NO_MATCH;
	} else if (memcmp(&buffer[4][0], &buffer[5][0], BUFFER_TRANSFER_SIZE)) {
		result = COMMON_ERR_NO_MATCH;
	} else if (memcmp(&buffer[6][0], &buffer[7][0], BUFFER_TRANSFER_SIZE)) {
		result = COMMON_ERR_NO_MATCH;
	} else if (memcmp(&buffer[8][0], &buffer[9][0], BUFFER_TRANSFER_SIZE)) {
		result = COMMON_ERR_NO_MATCH;
	} else {
		printf("\ndma_mem2mem_chaintransfer_test passed !\n");
		printf("New value of buffer[2][0]: %d \n", buffer[2][0]);
		printf("New value of buffer[3][0]: %d \n", buffer[3][0]);
		printf("New value of buffer[4][0]: %d \n", buffer[4][0]);
		printf("New value of buffer[5][0]: %d \n", buffer[5][0]);
		printf("New value of buffer[6][0]: %d \n", buffer[6][0]);
		printf("New value of buffer[7][0]: %d \n", buffer[7][0]);
		printf("New value of buffer[8][0]: %d \n", buffer[8][0]);
		printf("New value of buffer[9][0]: %d \n", buffer[9][0]);
		result = NO_ERROR;
	}
	return result;
}

int dma_mem2mem_newconfiguration_test(void) {
	int result = COMMON_ERR_UNKNOWN;
	mml_dma_status_t status;
	/* DMA configuration info */
	mml_dma_config_t config;
	/* DMA address */
	mml_dma_addr_t addr;

	/* Initialize the source memory buffer address with the know data */
	memset(&buffer[0][0], 0xAF, BUFFER_TRANSFER_SIZE);
	memset(&buffer[1][0], 0x00, BUFFER_TRANSFER_SIZE);

	printf("\n\nTest of dma_mem2mem_newconfiguration_test:\n");
	printf("This test does a mem2mem_transfer_test\n");
	printf("buffer[0][0]: %d\n", buffer[0][0]);
	printf("buffer[1][0]: %d\n", buffer[1][0]);
	/* Re-configure the DAM channel for new values */
	config.priority = DMAn_CFG_PRI_high;
	config.request = DMAn_CFG_REQSEL_memToMem;
	config.req_wait = MML_DMA_START_TIMER_NORMAL;
	config.timeout = DMAn_CFG_TOSEL_to4;
	config.prescale = DMAn_CFG_PSSEL_disable;
	config.src_width = DMAn_CFG_SRCWD_word;
	config.src_increment = 1;
	config.dst_width = DMAn_CFG_DSTWD_byte;
	config.dst_increment = 1;
	/* 0 to 31 (1 to 32 bytes) */
	config.burst_size = 31;
	result = mml_dma_set_configuration(channel, config);
	if (result) {
		printf(
				"DMA Usecase dma_mem2mem_newconfiguration_test Error: mml_dma_set_configuration()\n\r");
		return result;
	}
	addr.src = (unsigned int) &buffer[0][0];
	addr.dst = (unsigned int) &buffer[1][0];
	addr.count = BUFFER_TRANSFER_SIZE;
	result = mml_dma_set_address(channel, addr);
	if (result) {
		printf(
				"DMA Usecase dma_mem2mem_newconfiguration_test Error: mml_dma_set_address()\n\r");
		return result;
	}
	/* Enable the DMA channel transfer */
	result = mml_dma_enable(channel);
	if (result) {
		printf(
				"DMA Usecase dma_mem2mem_newconfiguration_test Error: mml_dma_enable()\n\r");
		return result;
	}
	/* Wait until DMA channel transfer completes */
	do {
		result = mml_dma_get_status(channel, &status);
		if (result) {
			printf(
					"DMA Usecase dma_mem2mem_newconfiguration_test Error: mml_dma_get_status()\n\r");
			return result;
		}
	} while (MML_DMA_STATUS_ENABLED == status);
	if (memcmp(&buffer[0][0], &buffer[1][0], BUFFER_TRANSFER_SIZE)) {
		result = COMMON_ERR_NO_MATCH;
	} else {
		printf("\ndma_mem2mem_newconfiguration_test passed !\n");
		printf("New value of buffer[0][0]: %d\n", buffer[0][0]);
		printf("New value of buffer[1][0]: %d\n", buffer[1][0]);
		result = NO_ERROR;
	}
	return result;
}

static void realod_callback(mml_dma_addr_t *rld_addr, int *reload) {
	static volatile int i = 5;

	++i;
	rld_addr->src = (unsigned int) &buffer[i][0];
	i++;
	rld_addr->dst = (unsigned int) &buffer[i][0];
	rld_addr->count = BUFFER_TRANSFER_SIZE;
	if (i <= 9) {
		*reload = 1;
	} else {
		*reload = 0;
		chain_transfer = 1;
	}
	return;
}

int main(void) {
	int result = COMMON_ERR_UNKNOWN;
	mml_uart_config_t config;

	/* Initialize UATR0 port with default configurations */
	config.baudrate = K_LITE_UART0_DEFAULT_BAUDRATE;
	config.data_bits = UARTn_CTRL_SIZE_bits8;
	config.flwctrl = UARTn_CTRL_RTSCTSF_disable;
	config.parity = MML_UART_PARITY_NONE;
	config.parity_mode = MML_UART_PARITY_MODE_ONES;
	config.rts_ctl = UARTn_PIN_RTS_lo;
	config.stop_bits = UARTn_CTRL_STOP_stop1;
	result = mml_uart_init(MML_UART_DEV0, config);
	if (result) {
		return result;
	}
	/* Welcome message */
	printf("\n\nWelcome to DMA usecase....");
	/* Perform DMA usecase tests */
	result = dma_tests();
	if (result) {
		printf("Failed !\n");
	} else {
		printf("\ndma_test Passed !\n");
	}

	return result;
}
#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__
/******************************************************************************/
/* EOF */
