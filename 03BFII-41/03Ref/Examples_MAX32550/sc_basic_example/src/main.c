/*
 * sc_basic_example/src/main.c emv_basic_example
 *
 ******************************************************************************
 * Copyright (C) 2012-2018 Maxim Integrated Products, Inc., All rights Reserved.
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
 * @defgroup EX_SC_BASIC Smart Card Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example show you how to use smart card api.
 *
 * This examples will print 10 ATR in a terminal via UART and then printf 5 ATR and 5 card response.
 *
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

/* Global includes */
#include <stdio.h>
#include <string.h>
#include <MAX325xx.h>
#include <errors.h>
#include <lib_emv/afe/MAX325xx_afe.h>
#include <lib_emv/OSWrapper.h>
#include <lib_emv/sc_errors.h>
#include <lib_emv/sc_types.h>
#include <lib_emv/smartcard_api.h>


#define EX_SC_BASIC_VERS_MAJOR	<VERSMAJ>
#define EX_SC_BASIC_VERS_MINOR	<VERSMIN>
#define EX_SC_BASIC_VERS_PATCH	<VERSPAT>
#define EX_SC_BASIC_VERSION_STRING	"v" xstr(EX_SC_BASIC_VERS_MAJOR) "." xstr(EX_SC_BASIC_VERS_MINOR) "." xstr(EX_SC_BASIC_VERS_PATCH)

#define	K_COBRA_LOCAL_BUF_SIZE_IN_BYTES		( 4 * 1024 )

typedef struct
{
	/* Read data buffer */
	unsigned char								buffer[K_COBRA_LOCAL_BUF_SIZE_IN_BYTES];
	/* Relative offset on next free room for data to receive */
	unsigned int								ofst_free;
	/* Relative offset on last data to read */
	unsigned int								ofst_last;

} uart_context_t;

uart_context_t context;

uint8_t			nbr_read_test1	  = 0xA;	/* Number of read for test1 */
uint8_t			nbr_read_test2	  = 0x5;	/* Number of read for test2 */

static uint8_t  command[256];
static uint8_t  response[258];	/* 256+ status bytes*/
static uint8_t  atr[32];

ActivationParams_t ActivationParams =
{
    .IccVCC = VCC_5V,
    .IccResetDuration = 112,    /* 41664 clock cycles*/
    .IccATR_Timeout   = 20160,  /* 20160 etus*/
    .IccTS_Timeout    = 114,    /* 42408 clock cycles*/
    .IccWarmReset     = 0,
};

const uint8_t selectFile[] =
{
    0x00, 0xA4, 0x04, 0x00,         /* select df file */
    14,                             /* payload length (Lc byte) */
    '1', 'P', 'A', 'Y', '.', 'S', 'Y', 'S', '.', 'D', 'D', 'F', '0', '1',
    0x00,                           /* answer length (Le byte) */
};

/* AFE USER Interrupt */
void __attribute__ ((optimize("O0"))) smart_card_inserted_interrupt(CardState_t cardstate)
{
	if (SC->SC_ISR & SC_SC_ISR_PDLIS_Msk)
	{
		if (cardstate == ICC_INSERTION) {
			printf("CARD INSERTED\n");
		}
		if (cardstate == ICC_REMOVAL) {
			printf("CARD REMOVED\n");
		}
		SC->SC_ISR |= 0 << SC_SC_ISR_PRCIS_Pos;
	}
	NVIC_ClearPendingIRQ(Smart_Card_IRQn);
}

void __attribute__ ((noinline))  __attribute__((optimize("-O0"))) Delay(unsigned int ms)
{
     unsigned int i = ms*5000;
     unsigned int systemfreq = 0;
     mml_get_system_frequency(&systemfreq);

     i /= (MML_SYSTEM_FREQUENCY_108_MHZ/systemfreq);

     for (; i != 0 ; i--)
          __asm volatile ("nop\n");
}

void uart0_handler(void)
{
    register unsigned int       isr;

    /* Retrieve ISR */
    isr = UART0->INT_STAT;
    /* Check ISR */
    if ( !( ( UARTn_INT_EN_FRAMIE_Msk |\
    		UARTn_INT_EN_PARITYIE_Msk |\
			UARTn_INT_EN_SIGNALIE_Msk |\
                UARTn_INT_EN_OVERIE_Msk ) & isr ) )
    {
        /* Get character */
        context.buffer[context.ofst_free++] = (unsigned char)UART0->DATA;
        /* update indexes */
        context.ofst_free %= K_COBRA_LOCAL_BUF_SIZE_IN_BYTES;
    }
    /* Clear all IRQ ... */
    mml_uart_interrupt_clear(MML_UART_DEV0, isr);
    /* Acknowledge interrupt at platform level */
    mml_uart_interrupt_ack(MML_UART_DEV0);
    return;
}

int init_uart(void)
{
    int                      result = COMMON_ERR_UNKNOWN;
    mml_uart_config_t        config;

    config.baudrate = 115200;
	config.data_bits = UARTn_CTRL_SIZE_bits8;
	config.flwctrl = UARTn_CTRL_RTSCTSF_disable;
	config.parity = MML_UART_PARITY_NONE;
	config.parity_mode = MML_UART_PARITY_MODE_ONES;
	config.rts_ctl = UARTn_PIN_RTS_lo;
	config.stop_bits = UARTn_CTRL_STOP_stop1;
    config.handler = (volatile mml_uart_handler_t)uart0_handler;
    result = mml_uart_init(MML_UART_DEV0, config);

    if ( NO_ERROR == result )
    {
        /* Clear all IRQ ... */
        mml_uart_interrupt_clear(MML_UART_DEV0, ( UARTn_INT_STAT_FRAMIS_Msk |\
        											UARTn_INT_STAT_PARITYIS_Msk |\
													UARTn_INT_STAT_SIGNALIS_Msk |\
                                                    UARTn_INT_STAT_OVERIS_Msk |\
                                                    UARTn_INT_STAT_FFRXIS_Msk |\
                                                    UARTn_INT_STAT_FFTXOIS_Msk |\
                                                    UARTn_INT_STAT_FFTXHIS_Msk ));
        /* ... set wanted interruption(s) */
        mml_uart_interrupt_set(MML_UART_DEV0, ( UARTn_INT_EN_FFRXIE_Msk |\
                                                    UARTn_INT_EN_FRAMIE_Msk |\
													UARTn_INT_EN_PARITYIE_Msk |\
													UARTn_INT_EN_SIGNALIE_Msk |\
                                                    UARTn_INT_EN_OVERIE_Msk ));
    }
    return result;
}

int main(void)
{
    /* Return value */
    int         result = COMMON_ERR_UNKNOWN;
    /* Local variables list */
    /* for smartcard tests */
    uint32_t    status = 0;
    uint32_t    len    = 0;
    uint32_t    i      = 0;
    IccReturn_t retval = ICC_OK;

    /* Initialize UART0 port with default configurations */
    result =  init_uart();
    if ( NO_ERROR == result )
    {
        /* Welcome message */
        printf("MAX32550 Smartcard basic test application\n");
    }

    /****************************/
    /* 		  FIRST TEST	    */
    /* 	Print CARD ATR 10 times	*/
    /****************************/
    printf("\n");
    printf("First Test: 10 read of ATR\n");
    printf("Insert a card\n");

    UartId_t curret_uartId	= SCI_0;
    MAX325xxSlots_t current_slot = SCI_0_INTERNAL_AFE;

    /* open slot INTERNAL_AFE for SC interfaces */
    SCAPI_open(curret_uartId, current_slot);

    /* Set the card frequency */
    status = 3600000; /*3.6MHz*/
    retval = SCAPI_ioctl(current_slot, IOCTL_SET_CLOCK_FREQ, &status);
    if (ICC_OK != retval)
    {
        goto power_off_1;
    }

	/* as the card has been powered off, we must reset
	 * the initparams, emv mode and working buffer
	 */

	/* Set the ATR timings  + card voltage */
	retval = SCAPI_ioctl(0, IOCTL_SET_INITPARAMS, (void *)&ActivationParams);
	if (ICC_OK != retval && retval != ICC_ERR_REMOVED)
	{
		goto power_off_1;
	}

	/* Attach AFE user interrupt */
	retval = SCAPI_ioctl(current_slot, IOCTL_SET_PRESENCE_IRQ, smart_card_inserted_interrupt);
	if (retval)
	{
		goto power_off_1;
	}

	/* Switch the stack to EMV mode*/
	status = 1;
	retval = SCAPI_ioctl(current_slot, IOCTL_SET_EMVMODE, &status);
	if (ICC_OK != retval)
	{
		goto power_off_1;
	}

    while(nbr_read_test1)
    {
        /* check if a card is present and NOT powered */
        retval = SCAPI_ioctl(current_slot, IOCTL_GET_CARD_STATE, &status);
        if ((ICC_OK != (IccReturn_t)retval) || (ICC_ERR_PRESENT_INACTIVE != (IccReturn_t)status))
        {
            goto power_off_1;
        }

    	status = POWER_UP;
        /* power up the card */
        ActivationParams.IccWarmReset = bFALSE;
        retval = SCAPI_ioctl(current_slot, IOCTL_POWER_CARD, &status);
        if (ICC_OK != retval)
        {
			goto power_off_1;
        }

        /*
         * Read the ATR and save into the atr buffer
         * as output, status will contains the exact ATR length
         */
		status = sizeof(atr);
        retval = SCAPI_read(current_slot, atr, &status);

        if (retval != ICC_OK)
        {
            goto power_off_1;
        }

        printf("\nCard ATR:\n");

        for (i=0; i<status; i++)
        {
            printf("%02X ", atr[i]);
        }
        printf("\n\n");

        /* Decrement number of read */
        nbr_read_test1--;

power_off_1:
		status = POWER_DOWN;
		/* power off the card */
		SCAPI_ioctl(current_slot, IOCTL_POWER_CARD, &status);
		Delay(10);
    }
	/* Close */
	SCAPI_close(current_slot);

    /****************************/
    /* 		  SECOND TEST	    */
    /* 	Print CARD ATR & card	*/
    /* 	  response 10 times		*/
    /****************************/
	printf("\n");
	printf("Second Test: 5 read of ATR and respond of card\n");
	printf("Insert a card\n");

    /* enable interrupts*/
    OSWrapper_Interrupt_enable();
    /* open slot INTERNAL_AFE for SC interfaces */
    SCAPI_open(curret_uartId, current_slot);

    /* set the card frequency */
    status = 3600000; /*3.6MHz*/
    retval = SCAPI_ioctl(current_slot, IOCTL_SET_CLOCK_FREQ, &status);
    if (ICC_OK != retval)
    {
        goto power_off;
    }

    memset(response, 0, sizeof(response));
    memset(command, 0, sizeof(command));
    memset(atr, 0, sizeof(atr));

    /* as the card has been powered off, we must reset
     * the initparams, emv mode and working buffer
     */
    /* set the ATR timings  + card voltage */
    retval = SCAPI_ioctl(current_slot, IOCTL_SET_INITPARAMS, (void *)&ActivationParams);
    if (ICC_OK != retval)
    {
        goto power_off;
    }

    /* Switch the stack to EMV mode*/
    status = 1;
    retval = SCAPI_ioctl(current_slot, IOCTL_SET_EMVMODE, &status);
    if (ICC_OK != retval)
    {
        goto power_off;
    }

    while(nbr_read_test2)
    {
        /* Default APDU Command = select PSE */
        memcpy(command, selectFile, sizeof(selectFile));
        len = sizeof(selectFile);

        /* check if a card is present and NOT powered */
        retval = SCAPI_ioctl(current_slot, IOCTL_GET_CARD_STATE, &status);
        if ((ICC_OK != (IccReturn_t)retval) ||
            (ICC_ERR_PRESENT_INACTIVE != (IccReturn_t)status))
        {
            goto power_off;
        }

        printf("A Card is present, powering it\n");

        status = POWER_UP;
        /* power up the card */
        ActivationParams.IccWarmReset = bFALSE;
        retval = SCAPI_ioctl(current_slot, IOCTL_POWER_CARD, &status);
        if (ICC_OK != retval)
        {
            goto power_off;
        }

        /*
         * Read the ATR and save into the atr buffer
         * as output, status will contains the exact ATR length
         */
        status = sizeof(atr);
        retval = SCAPI_read(current_slot, atr, &status);
        if (retval != ICC_OK)
        {
            goto power_off;
        }
        printf("Card ATR:\n");

        for (i=0; i<status; i++)
        {
            printf("%02X ", atr[i]);
        }
        printf("\n\n");

        /* Process a card exchange :
         * send the command and get the result into the
         * stack working buffer
         */
        printf("Send a select dedicated file (1PAY.SYS.DDF01):\n");

        for (i=0; i<len; i++)
        {
            printf("%02X ", command[i]);
        }
        printf("\n\n");

        retval = SCAPI_write(current_slot, command, len);
        if (ICC_OK != retval)
        {
            goto power_off;
        }

        /* read the answer from the stack working buffer */
        status = sizeof(response);
        retval = SCAPI_read(current_slot, response, &status);
        if (retval != ICC_OK)
        {
            goto power_off;
        }

        printf("Card response:\n");

        for (i=0; i<status; i++)
        {
            printf("%02X ", response[i]);
        }
        printf("\n\n");

        /* Decrement number of read */
        nbr_read_test2--;

power_off:
        status = POWER_DOWN;
        /* power off the card */
        SCAPI_ioctl(current_slot, IOCTL_POWER_CARD, &status);
    }
    SCAPI_close(current_slot);

    return result;
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__

/******************************************************************************/
/* EOF */
