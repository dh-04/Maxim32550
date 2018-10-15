/*
 * mlsOsalTestTaskRoundRobin.c
 *
 *  Created on: Dec 10, 2015
 *      Author: root
 */

#include "mlsOsalTest.h"

#if(MLS_OSAL_TEST)
#include "../../Unity/inc/unity.h"

static mlsTaskHandle_t task1, task2;
static UInt32 taskSTK1[128];
static UInt32 taskSTK2[128];

static volatile int gCounter = 0;

/****************************************************************************************************************
 *@ Function:
 *@ Brief:
 *@ Parameter:
 *@ Return value:
 */
static Void mlsTask1(Void* p_arg)
{
	MLS_UNUSED_PARAMETER(p_arg);

	while(1)
	{
		if(gCounter%2)	gCounter++;
//		mlsOsalSchedRoundRobinYield();
	}
}

/****************************************************************************************************************
 *@ Function:
 *@ Brief:
 *@ Parameter:
 *@ Return value:
 */
static Void mlsTask2(Void* p_arg)
{
	MLS_UNUSED_PARAMETER(p_arg);

	while(1)
	{
		if(!(gCounter%2)) gCounter++;
//		mlsOsalSchedRoundRobinYield();
	}
}

/****************************************************************************************************************
 *@ Function:
 *@ Brief:
 *@ Parameter:
 *@ Return value:
 */
/*static*/ mlsErrorCode_t mlsTestRoundRobin(Void)
{
	mlsErrorCode_t retVal;
	UInt8 index = 0;

	retVal = mlsOsalSchedRoundRobinCfg(MLSOSAL_ROUNDROBIN_ENABLE, 100);
	if(retVal != MLS_SUCCESS)
	{
		return retVal;
	}

	retVal = mlsOsalTaskCreate(&task1, mlsTask1, "Task 1", taskSTK1,128, (Void*)0, MLSOSAL_PRIO_ROUND_ROBIN);
	if(retVal != MLS_SUCCESS)
	{
		return retVal;
	}

	retVal = mlsOsalTaskCreate(&task2, mlsTask2, "Task 2", taskSTK2,128, (Void*)0, MLSOSAL_PRIO_ROUND_ROBIN);
	if(retVal != MLS_SUCCESS)
	{
		return retVal;
	}

	for(index = 0; index < (MLSOSAL_TEST_TIMEOUT/MLSOSAL_TEST_TIME_CHECK); index++)
	{
		if(gCounter > 10)
		{
			mlsOsalTaskDelete(&task1);
			mlsOsalTaskDelete(&task2);
			return MLS_SUCCESS;
		}

		mlsOsalDelayMs(MLSOSAL_TEST_TIME_CHECK);
	}
	mlsOsalTaskDelete(&task1);
	mlsOsalTaskDelete(&task2);

	return MLS_ERROR;
}

/****************************************************************************************************************
 *@ Function:
 *@ Brief:
 *@ Parameter:
 *@ Return value:
 */
Void mlsOsalTestRoundRobin(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsTestRoundRobin());
}

#endif //MLS_OSAL_TEST
/****************************************************************************************************************
 * ___________________________________________________END FILE__________________________________________________
 */
