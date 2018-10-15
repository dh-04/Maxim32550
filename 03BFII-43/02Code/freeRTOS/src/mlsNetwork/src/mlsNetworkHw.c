/*******************************************************************************
 *  (C) Copyright 2009 Molisys Solutions Co., Ltd. , All rights reserved       *
 *                                                                             *
 *  This source code and any compilation or derivative thereof is the sole     *
 *  property of Molisys Solutions Co., Ltd. and is provided pursuant to a      *
 *  Software License Agreement.  This code is the proprietary information      *
 *  of Molisys Solutions Co., Ltd and is confidential in nature.  Its use and  *
 *  dissemination by any party other than Molisys Solutions Co., Ltd is        *
 *  strictly limited by the confidential information provisions of the         *
 *  Agreement referenced above.                                                *
 ******************************************************************************/

/**
 * @defgroup mlsNetworkHw.c	mlsNetworkHw.c
 * @brief mlsNetworkHw.c
 *
 * Add more details about module
 * @{
 */

/**
 * @file	mlsNetworkHw.c
 *
 * @date 	8 Feb 2018
 * @author	stylvn008
 */


/********** Include section ***************************************************/
#include <errors.h>
#include <mml_gcr.h>
#include <mml_uart.h>
#include <mml_sflc_regs.h>
#include <mml_gcr_regs.h>
#include <mml_gpio.h>

#include "../inc/mlsNetwork.h"
#include "../inc/mlsNetworkHw.h"
#include "../mlsOsal/inc/mlsOsal.h"
#include "../mlsDebug/inc/mlsDebug.h"
/********** Local Constant and compile switch definition section **************/
#define NETWORK_HW_BAUDRATE	115200

#define NETWORK_HW_RESET_PIN		24
#define NETWORK_HW_RESET_PORT		MML_GPIO_DEV1
#define NETWORK_HW_POWER_PIN		23
#define NETWORK_HW_POWER_PORT		MML_GPIO_DEV1
/********** Local Type definition section *************************************/

/********** Local Macro definition section ************************************/

/********** Global variable definition section ********************************/
static fnNetworkProcessRsp gpNetworkProcessRsp = Null;
Bool gTxIsDone = True;
mlsMutexHandle_t gNetworkHwMutex;

/********** Local (static) variable definition section ************************/
static inline void UartRxInterrupt(void);
/********** Local (static) function declaration section ***********************/

/********** Local function definition section *********************************/
static inline void UartRxInterrupt(void)
{
	volatile unsigned int	isr;
	volatile mml_uart_regs_t	*reg_uart = (volatile mml_uart_regs_t*)MML_UART1_IOBASE;
	unsigned char data;

	/** Retrieve ISR */
	isr = reg_uart->isr;
	/** Clear all IRQ ... */
	reg_uart->isr &= ~(isr);

	/** Check ISR */
	if (isr & (MML_UART_ISR_OVERIS_MASK | MML_UART_ISR_FFRXIS_MASK))
	{
		while ((reg_uart->sr & MML_UART_SR_RXELT_MASK) != 0)
		{
			data = (unsigned char)reg_uart->dr;

			if (Null != gpNetworkProcessRsp)
			{
				gpNetworkProcessRsp(data);
			}
		}
	}

	/** We're done */
	return;
}

/********** Global function definition section ********************************/

mlsErrorCode_t mlsNetworkHwInit(fnNetworkProcessRsp handler)
{
	mlsErrorCode_t		result = COMMON_ERR_UNKNOWN;
	mml_uart_config_t 	config;
    static Bool IsNetworkHwInit = False;

	/* Assign function pointer to */
	gpNetworkProcessRsp = handler;

    if (IsNetworkHwInit)
    {
    	return MLS_SUCCESS;
    }
    IsNetworkHwInit = True;

	mlsOsalMutexCreate(&gNetworkHwMutex, "Network hw mutex");

	/** Initialize GPIO port */
	mml_gpio_config_t 	gpio_conf;

	gpio_conf.gpio_direction = MML_GPIO_DIR_OUT;
	gpio_conf.gpio_function = MML_GPIO_NORMAL_FUNCTION;
	gpio_conf.gpio_intr_mode = 0;
	gpio_conf.gpio_intr_polarity = 0;
	gpio_conf.gpio_pad_config = MML_GPIO_PAD_NORMAL;

	mml_gpio_init(NETWORK_HW_POWER_PORT, NETWORK_HW_POWER_PIN, 1, gpio_conf);
	mml_gpio_init(NETWORK_HW_RESET_PORT, NETWORK_HW_RESET_PIN, 1, gpio_conf);

	mml_gpio_write_bit_pattern(NETWORK_HW_RESET_PORT, NETWORK_HW_RESET_PIN, 1, 1);
	mml_gpio_write_bit_pattern(NETWORK_HW_RESET_PORT, NETWORK_HW_RESET_PIN, 1, 0);

	mlsNetworkHwPowerOn();

	/** Initialize UART */
	config.baudrate 	= NETWORK_HW_BAUDRATE;
	config.data_bits 	= MML_UART_DATA_TRANSFER_SIZE_8_BITS;
	config.flwctrl 		= MML_UART_HW_FLOW_CTL_DISABLE;
	config.parity 		= MML_UART_PARITY_NONE;
	config.parity_mode 	= MML_UART_PARITY_MODE_ONES;
	config.rts_ctl 		= MML_UART_RTS_IO_LEVEL_LOW;
	config.stop_bits 	= MML_UART_STOPBITS_ONE;
	config.handler 		= (volatile mml_uart_handler_t)UartRxInterrupt;

	result = mml_uart_init(MML_UART_DEV1, config);

	if ( NO_ERROR == result )
	{
		/** Disable interruption */
		result = M_MML_UART_INTERRUPT_DISABLE(MML_UART_DEV1);
		if ( result )
		{
			result = MLS_ERROR;
			goto echo_uart_init_out;
		}

		/** Clear all IRQ ... */
		mml_uart_interrupt_clear(MML_UART_DEV1, ( 	MML_UART_ISR_FRAMIS_MASK |\
													MML_UART_ISR_PARIS_MASK |\
													MML_UART_ISR_SIGIS_MASK |\
													MML_UART_ISR_OVERIS_MASK |\
													MML_UART_ISR_FFRXIS_MASK |\
													MML_UART_ISR_FFTXOIS_MASK |\
													MML_UART_ISR_FFTXHIS_MASK ));

		/** mls user just want these interruptions */
		mml_uart_interrupt_set(MML_UART_DEV1, ( MML_UART_IER_FFRXIE_MASK |\
												MML_UART_IER_OVERIE_MASK ));

	}
	/** We're done */
echo_uart_init_out:
	M_MML_UART_INTERRUPT_ENABLE(MML_UART_DEV1);

	return result;
}

mlsErrorCode_t mlsNetworkHwPowerOn()
{
#if DEBUG_LOG_NETWORK
	mlsDebugPrint("Turn on 3G/LTE module\r\n");
#endif

	mml_gpio_write_bit_pattern(NETWORK_HW_POWER_PORT, NETWORK_HW_POWER_PIN, 1, 0);
	mml_gpio_write_bit_pattern(NETWORK_HW_POWER_PORT, NETWORK_HW_POWER_PIN, 1, 1);
	mlsOsalDelayMs(500);
	mml_gpio_write_bit_pattern(NETWORK_HW_POWER_PORT, NETWORK_HW_POWER_PIN, 1, 0);

	return MLS_SUCCESS;
}

mlsErrorCode_t mlsNetworkHwReset()
{
#if DEBUG_LOG_NETWORK
	mlsDebugPrint("Network is reseting\r\n");
#endif

	mml_gpio_write_bit_pattern(NETWORK_HW_RESET_PORT, NETWORK_HW_RESET_PIN, 1, 1);

	if (gModule == ZTE_ME3630)
	{
		mlsOsalDelayMs(1000);
	}
	else
	{
		mlsOsalDelayMs(300);
	}

	mml_gpio_write_bit_pattern(NETWORK_HW_RESET_PORT, NETWORK_HW_RESET_PIN, 1, 0);
	if (gModule == ZTE_ME3630)
	{
		mlsDebugPrint("Please wait 32s...\r\n");
		mlsOsalDelayMs(32000);
	}
	else
	{
		mlsOsalDelayMs(5000);
	}

	return MLS_SUCCESS;
}

mlsErrorCode_t mlsNetworkHwWrite(UInt8 *writeBuf, UInt16 writeBufLen)
{
	mlsOsalMutexLock(&gNetworkHwMutex, MLS_OSAL_MAX_DELAY);

	UInt16 idx = 0;

    while (idx < writeBufLen)
    {
    	mml_uart_write_char(MML_UART_DEV1, writeBuf[idx]);
    	idx ++;
    }

	mlsOsalMutexUnlock(&gNetworkHwMutex);

	return MLS_SUCCESS;
}

mlsErrorCode_t mlsNetworkHwRead(UInt8 *readBuf, UInt16 *readBufLen)
{
	mlsErrorCode_t errorCode = MLS_SUCCESS;

	return errorCode;
}

mlsErrorCode_t mlsNetworkHwFlush()
{
	mlsErrorCode_t errorCode = MLS_SUCCESS;

	return errorCode;
}

/**@}*/
