/*******************************************************************************
* Copyright (C) 2018 Maxim Integrated Products, Inc., All rights Reserved.
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
*******************************************************************************
*/

#include "MAX325xx.h"
#include "utils.h"

#include <stdlib.h>

#define TRUE   1
#define FALSE  0

/***************************** VARIABLES *************************************/
static int isRtcStarted   = FALSE;

/************************   STATIC FUNCTIONS  *******************************/
static void rtc_start(void)
{
	/* First Disable/Reset RTC */
	mml_rtc_disable();
	mml_rtc_reset();
	/* Then start rtc */
	mml_rtc_start_up();

	isRtcStarted = TRUE;
}

/************************   PUBLIC FUNCTIONS  *******************************/
unsigned int utils_get_time_as_ms(void)
{
	unsigned int ms;

	if (isRtcStarted == 0) {
		rtc_start();
	}

	ms = (unsigned int) ((RTC->SEC * 1000) + (float)(3.9063 * RTC->SSEC)); // 1/256 = 0.0039063

	return ms;
}

unsigned int utils_get_time_tick(void)
{
	unsigned int tickCount;

	if (isRtcStarted == 0) {
		rtc_start();
	}

	tickCount = RTC->SEC;
	if (RTC->SSEC >= 128) {
		++tickCount;
	}

	return tickCount;
}

void utils_delay_ms(unsigned int ms)
{
	unsigned int startTime;

	startTime = utils_get_time_as_ms();
	while (utils_get_time_as_ms() < (startTime + ms))
		;

	return;
}

