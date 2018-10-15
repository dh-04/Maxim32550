/*
 * @file security_monitor/src/secure_monitor.c
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

/* Global includes */
#include <MAX325xx.h>
#include <errors.h>
#include <stdio.h>
/* Local includes */
#include <time.h>
#include <extscn.h>

unsigned int *ptr_NVSRAM=(unsigned int*)0x20081FFC;
int check_rtc_present_after_startup(void);
int extscn_debug_test(unsigned int cfg);
void processing(void);
int secure_application(void);

unsigned int magic_aes_key_value[] = { 0x1234, 0x5678, 0x9ABC, 0xDEF0 };

unsigned char extscn_status[NUMBER_OF_EXT_SENS];
unsigned char extscn_old_status[NUMBER_OF_EXT_SENS];

int check_rtc_present_after_startup(void) {
	/* RTC_RTCCN */
	if ( RTC->CN & RTC_CN_RTCE_Msk) {
		/* RTC running when check occurred */
		return COMMON_ERR_IN_PROGRESS;
	} else
		return NO_ERROR;
}

int extscn_debug_test(unsigned int cfg) {
	int result = 0;
	printf("Sensors Test...\n\n");
	mml_rtc_start_up();
	/* Wait for the busy flag */
	while ((( RTC->CN >> 3) & 0x1) == 1);
	/* Read back RTC control register */
	printf("RTC control register: 0x%08x\n\n", (unsigned int)RTC->CN);
	/* Wait for the busy flag */
	while ((( RTC->CN >> 3) & 0x1) == 1);
	printf("RTC second register: 0x%08x\n\n", (unsigned int)RTC->SEC);
	result = set_sensors(1, 7, 1, cfg, extscn_status);
	if(result)
		return result;
	printf("Sensors Enabled...\n\n");
	external_sens_tests(extscn_status, extscn_old_status, 1);

	return result;
}

/* Function too simulate operation */
void processing(void) {
	char prog[4] = { '-', '/', '|', '\\' };
	int t = 0;

	while (1) {
		for (int i = 0; i < 0xffff; i++);
		printf("\r%c", prog[t % 4]);
		fflush(0);
		t++;
	}
}

int secure_application(void) {
	int result = COMMON_ERR_UNKNOWN;
	unsigned int i;

	/* Basic initialization */
	for (i = 0; i < NUMBER_OF_EXT_SENS; i++) {
		extscn_status[i] = IDLE;
	}
	/*
	 * First check BPK AES Key / DRS status
	 * Value is normally 0x0 after reset due to RTC is not started
	 */
	/* Test BORF bit */
	if (SMON->SECDIAG & SMON_SECDIAG_BORF_Msk) {
		printf("First system startup after power-on.\n");

		printf("\nWrite of magic word 0xDEADBEEF at end of NVSRAM\n");
		(*ptr_NVSRAM) = 0xDEADBEEF;
		printf("0x%08X=0x%08X\n\n",(unsigned int)ptr_NVSRAM, (unsigned int)(*ptr_NVSRAM));

		reload_system_after_drs();
		/* Set BORF bits to 0 */
		SMON->SECDIAG &= ~SMON_SECDIAG_BORF_Msk;
		processing();
	} else {
	result = check_rtc_present_after_startup();
	if (result) {
		printf("RESET Occurred.\n");
		/* It is highly recommended to load AES key in secure environment.
		* Doing an applet for load AES Key for example.
		*/
		result = check_aes_key();
		if (NO_ERROR == result) {
			printf("KEY was successfully erased.\n");

			/* It is highly recommended to load AES key in secure environment.
			 * Doing an applet for load AES Key for example.
			 */
			printf("\nRead of magic word at end of NVSRAM:\n");
			printf("0x%08X=0x%08X\n\n",(unsigned int)ptr_NVSRAM, (unsigned int)(*ptr_NVSRAM));

			reload_system_after_drs();

			printf("RELOAD AES Key.\n");
			load_aes_key();

			printf("\nRead of magic word at end of NVSRAM:\n");
			printf("0x%08X=0x%08X\n\n",(unsigned int)ptr_NVSRAM, (unsigned int)(*ptr_NVSRAM));

			processing();
		} else {
			printf("KEY was NOT erased !!!!\n");
			result = COMMON_ERR_FATAL_ERROR;
			return result;
		}
	} else
			printf("Second system startup after power-on.\n");
	}
	extscn_debug_test(EXT_SENS_MASK);
	result = external_sens_tests(&extscn_status[0], &extscn_old_status[0], 1);
	processing();
	return result;
}

/* EOF */
