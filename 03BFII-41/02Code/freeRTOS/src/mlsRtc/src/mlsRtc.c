/**
 * @addtogroup mlsRtc
 * @{
 */

/**
 * @file mlsRtc.c
 * @brief RTC component Implementation
 *
 *
 * @date 	$ March, 2016
 * @author	$
 */

/********** Include section ***************************************************/
#include "../inc/mlsRtc.h"

#include <mml_rtc_regs.h>
#include "mml_gpio.h"
#include "mml_uart.h"
#include "mml_gcr.h"

#include "mlsInclude.h"
//#include "../mlsInfo/inc/mlsInfo.h"

/********** Local Constant and compile switch definition section **************/
/*RTC*/
#define RTC_BASE 		0x40006000
#define RTC_RTCSEC		RTC_BASE + 0x0
#define RTC_RTCSSEC		RTC_BASE + 0x04
#define RTC_RTCCN		RTC_BASE + 0x10

/********** Local Type definition section *************************************/

/********** Local Macro definition section ************************************/

/********** Local (static) variable definition section ************************/
volatile mml_rtc_regs_t *rtc_regs = (volatile mml_rtc_regs_t *)MML_RTC_IOBASE;

/********** Local (static) function declaration section ***********************/

/********** Local function definition section *********************************/

/********** Global function definition section ********************************/
mlsMutexHandle_t gMutexRtc;

/******************************************************************************
 * mlsRtcGet
 ******************************************************************************/
mlsErrorCode_t mlsRtcGet(UInt32 *valueRtc, mlsTimeType_t type)
{
	mlsOsalMutexLock(&gMutexRtc, MLS_OSAL_MAX_DELAY);

	/* Check RTC busy bit */
	while (((rtc_regs->cr >> MML_RTC_CR_BUSY_OFST) & MML_RTC_CR_BUSY_MASK) == 1);

	if (((rtc_regs->cr >> MML_RTC_CR_RTCE_OFST) & MML_RTC_CR_RTCE_MASK) == 1)//Real time clock enabled
	{
		/* Check RTC Ready bit */
		while (((rtc_regs->cr >> MML_RTC_CR_RDY_OFST) & MML_RTC_CR_RDY_MASK) == 0);
	}

	*valueRtc = rtc_regs->sec;

	if(TIME_LOCAL == type)
	{
		*valueRtc += (mlsInfoSettings.Settings.TimeZone.value);
	}

	mlsOsalMutexUnlock(&gMutexRtc);

	return MLS_SUCCESS;
}

/******************************************************************************
 * mlsRtcSet
 ******************************************************************************/
mlsErrorCode_t mlsRtcSet(UInt32 valueRtc, mlsPriority_t prior)
{
	static UInt8 isPermit = true;

	mlsOsalMutexLock(&gMutexRtc, MLS_OSAL_MAX_DELAY);

	if(PRIOR_HIGH == prior)
	{
		isPermit = false;
	}

	if(true == isPermit || PRIOR_HIGH == prior)
	{
		/* reset RTC Control register and write enable */
		// Check RTC busy bit
		while (((rtc_regs->cr >> MML_RTC_CR_BUSY_OFST) & MML_RTC_CR_BUSY_MASK) == 1);
		rtc_regs->cr = (0x01 & MML_RTC_CR_WE_MASK) << MML_RTC_CR_WE_OFST;


		/* Reset RTC second register */
		// Check RTC busy bit
		while (((rtc_regs->cr >> MML_RTC_CR_BUSY_OFST) & MML_RTC_CR_BUSY_MASK) == 1);
		rtc_regs->sec = valueRtc;

		/* Reset RTC sub-second register */
		// Check RTC busy bit
		while (((rtc_regs->cr >> MML_RTC_CR_BUSY_OFST) & MML_RTC_CR_BUSY_MASK) == 1);
		rtc_regs->ssec = 0x00000000;

		/* Enable RTC and always operate in Quite Mode*/
		// Check RTC busy bit
		while (((rtc_regs->cr >> MML_RTC_CR_BUSY_OFST) & MML_RTC_CR_BUSY_MASK) == 1);
		rtc_regs->cr = 0x00000801;
	}

	mlsOsalMutexUnlock(&gMutexRtc);

	return MLS_SUCCESS;
}
