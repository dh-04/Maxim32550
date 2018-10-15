/*
 * mlsOsalTestMutex.c
 *
 *  Created on: Dec 7, 2015
 *      Author: root
 */

#include "mlsOsalTest.h"

#if(MLS_OSAL_TEST)
#include "../../Unity/inc/unity.h"
#include "../mlsDebug/inc/printf_lite.h"

static mlsMutexHandle_t	myMutex;
static mlsTaskHandle_t		mutexTask1, mutexTask2;
static UInt32 mutexSTK1[128];
static UInt32 mutexSTK2[128];

static Bool gMutexIsWorked = False;

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static mlsErrorCode_t mlsCreateMutex(Void)
{
	return (mlsOsalMutexCreate(&myMutex, "My MUTEX"));
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static Void mlsMutexTask1(Void* p_arg)
{
	MLS_UNUSED_PARAMETER(p_arg);

	while(1)
	{
		mlsOsalMutexLock(&myMutex, MLS_OSAL_MAX_DELAY);

		mlsOsalMutexUnlock(&myMutex);
		mlsOsalDelayMs(250);
	}
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static Void mlsMutexTask2(Void* p_arg)
{
	MLS_UNUSED_PARAMETER(p_arg);

	while(1)
	{
		mlsOsalMutexLock(&myMutex, MLS_OSAL_MAX_DELAY);

		if(!gMutexIsWorked)
		{
			gMutexIsWorked = True;
		}

		mlsOsalMutexUnlock(&myMutex);
		mlsOsalDelayMs(250);
	}
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static mlsErrorCode_t mlsCheckMutexIsWorked(Void)
{
	mlsErrorCode_t retVal;
	UInt8 index = 0;

	retVal = mlsOsalTaskCreate(&mutexTask1,
							   mlsMutexTask1,
							   "Task 1",
							   mutexSTK1,
							   128,
							   (Void*)0,
							   MLSOSAL_PRIO_OTHER_TASK_TEST);
	if(retVal != MLS_SUCCESS)
	{
		return MLS_ERROR;
	}

	retVal = mlsOsalTaskCreate(&mutexTask2,
							   mlsMutexTask2,
							   "Task 2",
							   mutexSTK2,
							   128,
							   (Void*)0,
							   MLSOSAL_PRIO_OTHER_TASK_TEST);
	if(retVal != MLS_SUCCESS)
	{
		return MLS_ERROR;
	}

	for(index = 0; index < (MLSOSAL_TEST_TIMEOUT/MLSOSAL_TEST_TIME_CHECK); index++)
	{
		if(gMutexIsWorked)
		{
			mlsOsalTaskDelete(&mutexTask1);
			mlsOsalTaskDelete(&mutexTask2);
			return MLS_SUCCESS;
		}

		mlsOsalDelayMs(MLSOSAL_TEST_TIME_CHECK);
	}
	mlsOsalTaskDelete(&mutexTask1);
	mlsOsalTaskDelete(&mutexTask2);

	return MLS_ERROR;
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static mlsErrorCode_t mlsDeleteMutex(Void)
{
	return mlsOsalMutexDelete(&myMutex);
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
Void mlsOsalTestCreateMutex(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsCreateMutex());
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
Void mlsOsalTestMutexIsWorked(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsCheckMutexIsWorked());
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
Void mlsOsalTestMutexDelete(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsDeleteMutex());
}

#endif //MLS_OSAL_TEST
/************************************************************************************************************
 * _______________________________________________END FILE__________________________________________________
 */
