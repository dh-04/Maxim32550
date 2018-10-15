/*
 * mlsOsalTestTaskChangePriority.c
 *
 *  Created on: Dec 10, 2015
 *      Author: root
 */

#include "mlsOsalTest.h"

#if(MLS_OSAL_TEST)
#include "../../Unity/inc/unity.h"
#include "../mlsDebug/inc/printf_lite.h"

static mlsTaskHandle_t pTask1, pTask2;
static UInt32 taskSTK1[128];
static UInt32 taskSTK2[128];
static Bool gTask1IsRunning = False;
static Bool gTask2IsRunning = False;

/****************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static Void mlsTask1(Void* p_arg)
{
	MLS_UNUSED_PARAMETER(p_arg);

	while(1)
	{
		if(!gTask1IsRunning)	gTask1IsRunning = True;
	}
}

/****************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static Void mlsTask2(Void* p_arg)
{
	MLS_UNUSED_PARAMETER(p_arg);

	while(1)
	{
		if(gTask1IsRunning == True && gTask2IsRunning == False)
		{
			gTask1IsRunning = False;
			gTask2IsRunning = True;
		}
	}
}

/****************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static mlsErrorCode_t mlsOsalTestTaskChangePrio(Void)
{
	mlsErrorCode_t retVal;
	UInt8 index = 0;

	retVal = mlsOsalTaskCreate(&pTask1, mlsTask1, "Task 1", taskSTK1, 128, (Void*)0, MLSOSAL_PRIO_TEST_CHANGE_PRIO_1);
	if(retVal != MLS_SUCCESS)
	{
		return retVal;
	}

	retVal = mlsOsalTaskCreate(&pTask2, mlsTask2, "Task 2", taskSTK2, 128, (Void*)0, MLSOSAL_PRIO_TEST_CHANGE_PRIO_2);
	if(retVal != MLS_SUCCESS)
	{
		return retVal;
	}

	static int count = 0;
	for(index = 0; index < (MLSOSAL_TEST_TIMEOUT/MLSOSAL_TEST_TIME_CHECK); index++)
	{
		if(gTask1IsRunning == False && gTask2IsRunning == True)
		{
			count++;
			if(count > 1)
			{
				count = 0;
				mlsOsalTaskDelete(&pTask1);
				mlsOsalTaskDelete(&pTask2);
				return MLS_SUCCESS;
			}
		}
		mlsOsalDelayMs(MLSOSAL_TEST_TIME_CHECK);
	}
	mlsOsalTaskDelete(&pTask1);
	mlsOsalTaskDelete(&pTask2);

	return MLS_ERROR;
}

/****************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsOsalTestChangePrio(Void)
{
	lite_printf(DBG_PRINT_LEVEL_DEBUG_MASK, "\r\nPRESS KEY (v) to change Priority!\r\n");
	lite_printf(DBG_PRINT_LEVEL_DEBUG_MASK, "10s - Waiting...\r\n");

	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsOsalTestTaskChangePrio());
}

/****************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsChangePrioTask1(Void)
{
	mlsOsalTaskChangePrio(&pTask1, MLSOSAL_PRIO_TO_CHANGE);
}

#endif //MLS_OSAL_TEST
/****************************************************************************************************************
 * ______________________________________________________END FILE_______________________________________________
 */
