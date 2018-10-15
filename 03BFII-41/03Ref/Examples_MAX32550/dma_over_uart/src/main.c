/*
 * @file dma_over_uart/src/main.c
 *
 ******************************************************************************
 * Copyright (C) 2017-2018 Maxim Integrated Products, Inc., All rights Reserved.
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
 * @brief This example show you how to configure DMA in UART Rx mode
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <MAX325xx.h>
#include <string.h>

#define EX_DMA_VERS_MAJOR	<VERSMAJ>
#define EX_DMA_VERS_MINOR	<VERSMIN>
#define EX_DMA_VERS_PATCH	<VERSPAT>
#define EX_DMA_VERSION_STRING	"v" xstr(EX_DMA_VERS_MAJOR) "." xstr(EX_DMA_VERS_MINOR) "." xstr(EX_DMA_VERS_PATCH)

#define COM_PORT   				MML_UART_DEV0
#define	BUFFER_TRANSFER_SIZE	4  // set number of byte

static int init_uart(void)
{
	int result = COMMON_ERR_UNKNOWN;
	mml_uart_config_t config;

	config.baudrate 	= 115200;
	config.data_bits 	= UARTn_CTRL_SIZE_bits8;
	config.flwctrl 		= UARTn_CTRL_RTSCTSF_disable;
	config.parity 		= MML_UART_PARITY_NONE;
	config.parity_mode 	= MML_UART_PARITY_MODE_ONES;
	config.rts_ctl 	 	= UARTn_PIN_RTS_lo;
	config.stop_bits 	= UARTn_CTRL_STOP_stop1;
	config.handler		= NULL;

	result = mml_uart_init(COM_PORT, config);

	return result;
}

static int init_dma(mml_dma_channel_t ch, mml_dma_addr_t addr)
{
	int 			  result = COMMON_ERR_UNKNOWN;
	mml_dma_config_t  config; 					   /* DMA configuration info */

	config.priority 	 = DMAn_CFG_PRI_low;
	config.request 		 = DMAn_CFG_REQSEL_uart0Rx;
	config.req_wait 	 = MML_DMA_DELAY_TIMER_START;
	config.timeout 		 = DMAn_CFG_TOSEL_to4;
	config.prescale 	 = DMAn_CFG_PSSEL_div64k;
	config.src_width 	 = DMAn_CFG_SRCWD_byte;
	config.src_increment = 0;
	config.dst_width 	 = DMAn_CFG_DSTWD_byte;
	config.dst_increment = 1;
	config.burst_size 	 = 1;
	config.handler 	     = NULL;

	/* Reset DMA interface */
	GCR->RSTR |= GCR_RSTR_DMA_Msk;
	/* Wait until DMA reset completes */
	while ( GCR_RSTR_DMA_Msk & GCR->RSTR);

	/* Initialize DMA channel */
	result = mml_dma_init_without_irq(ch, config, addr);

	return result;
}

int main(void)
{
	int  			  result = COMMON_ERR_UNKNOWN;
	unsigned int	  i;
	unsigned int 	  rcv_len;
	char 			  buffer[BUFFER_TRANSFER_SIZE]; /** Memory buffers for DMA transfers */
	mml_dma_channel_t dma_ch;
	mml_dma_addr_t    addr; 				           /* DMA address */
	mml_dma_status_t  status;

	/** Initialize the memory buffer to zero */
	memset(&buffer[0],  0, sizeof(buffer));
	// set address
	addr.dst   = (unsigned int) &buffer[0];
	addr.count = BUFFER_TRANSFER_SIZE;

	/* Initialize UART0 port with default configurations */
	result = init_uart();
	if (result)
		return result;

	/* Get free channel DMA */
	dma_ch = mml_dma_get_free_channel();
	/* Initialize DMA channel */
	result = init_dma(dma_ch, addr);
	if (result)
		return result; /* Failed to initialize DMA channel */

	printf("\nWelcome to DMA over UART application.\n");
	printf("In this example UART is configured in DMA mode,\n");
	printf("and it works in loopback mode over %d bytes\n", BUFFER_TRANSFER_SIZE);
	printf("In idle state board will send 0\n");
	printf("--------------------------------------------------------\n");
	// set the UART in DMA Rx Mode
	UART0->DMA_REG |= UARTn_DMA_REG_RXEN_Msk;
	// enable dma
	mml_dma_enable(dma_ch);

	while (1) {

		/* Wait until DMA channel transfer completes */
		i = 0 ;
		do {
			result = mml_dma_get_status(dma_ch, &status);
			if (result)
				return result; // DMA Usecase Error

			if (++i > 3000000) {
				mml_uart_write_char(COM_PORT , '0'); // dummy out to prove it is working...
				i = 0;
			}
		} while (MML_DMA_STATUS_ENABLED == status);

		mml_dma_disable(dma_ch);

		/*** print receive bytes ***/
		rcv_len = BUFFER_TRANSFER_SIZE - (DMAC->DMA_CH[dma_ch].DMAn_CNT);
		for ( i = 0; i < rcv_len; i++)
			mml_uart_write_char(COM_PORT, buffer[i]);

		/*** restart dma ***/
		mml_dma_set_address(dma_ch, addr);
		mml_dma_enable(dma_ch);
	}

	return result;
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__

/******************************************************************************/
/* EOF */
