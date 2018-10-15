/*
 * @file dma_usecases/include/dma.h
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
 * trademarks, Mskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 ******************************************************************************
 */

/** Global includes */
#include <MAX325xx.h>
#include <errors.h>
#include <stdio.h>
#include <string.h>

/** DMA transfer size*/
#define	BUFFER_TRANSFER_SIZE	1024

/* UART configuration */
#define	K_LITE_MAX_SYST_FREQ_FOR_UART		192000000
#define	K_LITE_UART0_DEFAULT_BAUDRATE		115200
#define LITE_UART_BAUD_RATE_MIN				1200
#define LITE_UART_BAUD_RATE_MAX				921600

/** Memory buffers for DMA transfers */
char buffer[10][BUFFER_TRANSFER_SIZE];
static void realod_callback(mml_dma_addr_t *rld_addr, int *reload);



int dma_tests(void);


/**
 * DMA usecase to test memory-to-memory data transfers.
 * @param			None
 * @retval  	  	0: test passed
 * @retval			-1 : test case failure
 */
int dma_mem2mem_transfer_test(void);


/**
 * DMA usecase to test memory-to-memory chained buffer data transfers with same
 * DMA configurations.
 * @param			None
 * @retval  	  	0: test passed
 * @retval			-1 : test case failure
 */
int dma_mem2mem_chaintransfer_test(void);


/**
 * Callback function for chained buffer transfers
 * @param rld_addr
 * @param reload
 */
/** Following function is called from the interrupt context */
static void realod_callback(mml_dma_addr_t *rld_addr, int *reload);


/**
 * DMA usecase to test memory-to-memory data transfer with new configuration.
 * @param			None
 * @retval  	  	0: test passed
 * @retval			-1 : test case failure
 */
int dma_mem2mem_newconfiguration_test(void);


/* EOF */
