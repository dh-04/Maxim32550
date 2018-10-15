/*
 * @file main.c emv_loopback
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
 * @defgroup EX_SC_EMV_LOOP_BACK EMV Loopback Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example is used for EMV certification.
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */
 
/* Global includes */
#include <MAX325xx.h>
#include <string.h>
#include <errors.h>

/* Smartcard specific */
#include "lib_emv/afe/MAX325xx_afe.h"
#include "sc_errors.h"
#include "sc_types.h"
#include "smartcard_api.h"


#define EX_SC_EMV_LOOP_BACK_VERS_MAJOR	<VERSMAJ>
#define EX_SC_EMV_LOOP_BACK_VERS_MINOR	<VERSMIN>
#define EX_SC_EMV_LOOP_BACK_VERS_PATCH	<VERSPAT>
#define EX_SC_EMV_LOOP_BACK_VERSION_STRING	"v" xstr(EX_SC_EMV_LOOP_BACK_VERS_MAJOR) "." xstr(EX_SC_EMV_LOOP_BACK_VERS_MINOR) "." xstr(EX_SC_EMV_LOOP_BACK_VERS_PATCH)


#define INTER_APDU_DELAY     500 /* 500ms between each APDU request*/

static uint8_t  command[256];
static uint8_t  response[258]; /* 256 + status bytes*/

ActivationParams_t ActivationParams = {
    .IccVCC = VCC_5V,
    .IccResetDuration = 108,    /* 41664 clock cycles*/
    .IccATR_Timeout   = 20160,  /* 20160 etus*/
    .IccTS_Timeout    = 114,    /* 42408 clock cycles*/
    .IccWarmReset     = 0,
};

const uint8_t selectFile[] = {
    0x00, 0xA4, 0x04, 0x00,         /* select df file */
    14,                             /* payload length (Lc byte) */
    '1', 'P', 'A', 'Y', '.', 'S', 'Y', 'S', '.', 'D', 'D', 'F', '0', '1',
    0x00,                           /*answer length (Le byte) */
};

void __attribute__ ((noinline))  __attribute__((optimize("-O0"))) Delay(unsigned int ms)
{
     unsigned int i = ms*5000;
     unsigned int systemfreq = 0;
     mml_get_system_frequency(&systemfreq);

     i /= (MML_SYSTEM_FREQUENCY_108_MHZ/systemfreq);

     for (; i != 0 ; i--)
          __asm volatile ("nop\n");

}

int main(void) {
    /* Return value */
    int			result = COMMON_ERR_UNKNOWN;
    /* Local variables list */
    /* for smartcard tests */
    uint32_t    status = 0;
    uint32_t    len    = 0;
    IccReturn_t retval = ICC_OK;
    /* Select SC0 interface and internal AFE to use Smart card reader
     * Select SC1 interface and internal AFE to use SAM card
     */
    UartId_t curret_uartId	= SCI_0;
    MAX325xxSlots_t current_slot = SCI_0_INTERNAL_AFE;
    /* enable the interrupts*/
    __enable_irq();
    /* open slot 0 */
    SCAPI_open(curret_uartId, current_slot);
    /* set the card frequency */
    status = 4000000; /*4MHz*/
    retval = SCAPI_ioctl(current_slot, IOCTL_SET_CLOCK_FREQ, &status);
    if (ICC_OK != retval) {
        goto power_off;
    }
    while( 1 ) {
        memset(response, 0, sizeof(response));
        memset(command, 0, sizeof(command));

        /* Default APDU Command = select PSE */
        memcpy(command, selectFile, sizeof(selectFile));
        len = sizeof(selectFile);

        /* check if a card is present and NOT powered */
        retval = SCAPI_ioctl(current_slot, IOCTL_GET_CARD_STATE, &status);
        if ((ICC_OK != retval) ||
            (ICC_ERR_PRESENT_INACTIVE != (IccReturn_t)status)) {
            goto power_off;
        }
        /*restore card parameters (EMV Mode and initial params) */
        /* set the ATR timings  + card voltage */
        retval = SCAPI_ioctl(current_slot, IOCTL_SET_INITPARAMS, (void *)&ActivationParams);
        if (ICC_OK != retval) {
            goto power_off;
        }
        /*
         * Switch the stack to EMV mode
         * This must be done each time you closed a card session
         */
        status = 1;
        retval = SCAPI_ioctl(current_slot, IOCTL_SET_EMVMODE, &status);
        if (ICC_OK != retval) {
            goto power_off;
        }

        status = POWER_UP;
        /*power up the card */
        ActivationParams.IccWarmReset = bFALSE;
        retval = SCAPI_ioctl(current_slot, IOCTL_POWER_CARD, &status);
        if (ICC_OK != retval) {
            goto power_off;
        }
        while (1) {
                /* Process a card exchange :
                 * send the command and get the result into the
                 * stack working buffer
                 */
                retval = SCAPI_write(current_slot, command, len);
                if (ICC_OK != retval) {
                    break;
                }

                /* read the answer from the stack working buffer */
                status = sizeof(response);
                retval = SCAPI_read(current_slot, response, &status);
                if (retval != ICC_OK) {
                    break;
                }

                /*
                 * Per the lab request, we must wait at least 200ms between each
                 * APDU exchange in order to let the test tool collect the data
                 */
                Delay(INTER_APDU_DELAY);

                /* Prepare next APDU */
                if (status < 6) {
                    /* Default APDU Command = select PSE */
                    memcpy(command, selectFile, sizeof(selectFile));
                    len = sizeof(selectFile);
                } else if(response[1] == 0x70) {
                    /* Abort loopback */
                    break;
                } else {
                    /* Loopback */
                    len = status -2;
                    memcpy(command, response, len);
                }
        }
power_off:
        status = POWER_DOWN;
        /*power off the card */
        SCAPI_ioctl(current_slot, IOCTL_POWER_CARD, &status);

        Delay(2000);
    }
    SCAPI_close(current_slot);
    return result;
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__

/******************************************************************************/
/* EOF */
