/*
 * mlsOsalTestTaskPremp.c
 *
 *  Created on: Dec 10, 2015
 *      Author: root
 */

#include "mlsOsalTest.h"

#if(MLS_OSAL_TEST)
#include "../../Unity/inc/unity.h"

static mlsTaskHandle_t	taskHandler1, taskHandler2;
static UInt32 taskSTK1[128];
static UInt32 taskSTK2[128];

static volatile int gCountHigh 	= 0;
static volatile int gCountLow 	= 0;
/****************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static Void pTaskHigh(Void* p_arg)
{
	MLS_UNUSED_PARAMETER(p_arg);

	gCountHigh = 0;

	while(1)
	{
		gCountHigh++;
		mlsOsalDelayMs(100);
	}
}

/****************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static Void pTaskLow(Void* p_arg)
{
	MLS_UNUSED_PARAMETER(p_arg);
	gCountLow = 0;

	while(1)
	{
		gCountLow++;
		// DO Nothing
	}
}

/****************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
/*static*/ mlsErrorCode_t mlsTestTaskPreemp(Void)
{
	mlsErrorCode_t retVal;
	UInt8 index = 0;

	retVal = mlsOsalTaskCreate(&taskHandler1,
							   pTaskHigh,
							   "pTaskHigh",
							   taskSTK1,
							   128,
							   (Void*)0,
							   MLSOSAL_PRIO_PREEMP_HIGH);
	if(retVal != MLS_SUCCESS)
	{
		return retVal;
	}


	retVal = mlsOsalTaskCreate(&taskHandler2,
							   pTaskLow,
							   "pTaskLow",
							   taskSTK2,
							   128,
							   (Void*)0,
							   MLSOSAL_PRIO_PREEMP_LOW);
	if(retVal != MLS_SUCCESS)
	{
		return retVal;
	}

	for(index = 0; index < (MLSOSAL_TEST_TIMEOUT/MLSOSAL_TEST_TIME_CHECK); index++)
	{
		if(gCountHigh > 5 && gCountLow > 5)
		{
			mlsOsalTaskDelete(&taskHandler1);
			mlsOsalTaskDelete(&taskHandler2);

			return MLS_SUCCESS;
		}

		mlsOsalDelayMs(MLSOSAL_TEST_TIME_CHECK);
	}

	mlsOsalTaskDelete(&taskHandler1);
	mlsOsalTaskDelete(&taskHandler2);

	return MLS_ERROR;
}
/****************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsOsalTestPreemp(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsTestTaskPreemp());
}

#endif //MLS_OSAL_TEST
/****************************************************************************************************************
 * ___________________________________________________END_FILE__________________________________________________
 */
