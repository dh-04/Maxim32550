/*
 * mlsOsalTestSem.c
 *
 *  Created on: Dec 7, 2015
 *      Author: root
 */

#include "mlsOsalTest.h"

#if(MLS_OSAL_TEST)

#include "../../Unity/inc/unity.h"
#include "../mlsDebug/inc/printf_lite.h"

static mlsSemHandle_t mySem;
static mlsTaskHandle_t	semTask1, semTask2, semTask3, semTask4;
static UInt32 semSTK1[128];
static UInt32 semSTK2[128];
static UInt32 semSTK3[128];
static UInt32 semSTK4[128];

static volatile int gSemCounter1 = 0;
static volatile int gSemCounter2 = 0;
static volatile int gSemCounter3 = 0;

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static mlsErrorCode_t mlsCreateSem(Void)
{
	return (mlsOsalSemCreate(&mySem, "My SEM", 3, 0));
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static Void mlsSemTask1(Void* p_arg)
{
	MLS_UNUSED_PARAMETER(p_arg);

	while(1)
	{
		mlsOsalSemGet(&mySem, MLS_OSAL_MAX_DELAY);

		gSemCounter1++;

		mlsOsalDelayMs(100);
	}
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static Void mlsSemTask2(Void* p_arg)
{
	MLS_UNUSED_PARAMETER(p_arg);

	while(1)
	{
		mlsOsalSemGet(&mySem, MLS_OSAL_MAX_DELAY);

		gSemCounter2++;

		mlsOsalDelayMs(100);
	}
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static Void mlsSemTask3(Void* p_arg)
{
	MLS_UNUSED_PARAMETER(p_arg);

	while(1)
	{
		mlsOsalSemGet(&mySem, MLS_OSAL_MAX_DELAY);

		gSemCounter3++;

		mlsOsalDelayMs(100);
	}
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static Void mlsSemTask4(Void* p_arg)
{
	MLS_UNUSED_PARAMETER(p_arg);

	while(1)
	{
		mlsOsalSemSet(&mySem);

		mlsOsalDelayMs(250);
	}
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static mlsErrorCode_t mlsCheckSemIsWorked(Void)
{
	mlsErrorCode_t retVal;
	UInt8 index = 0;

	retVal = mlsOsalTaskCreate(&semTask1, mlsSemTask1, "Sem Task 1",
							   semSTK1, 128, (Void*)0,
							   MLSOSAL_PRIO_OTHER_TASK_TEST);
	if(retVal != MLS_SUCCESS)
		return MLS_ERROR;

	retVal = mlsOsalTaskCreate(&semTask2, mlsSemTask2, "Sem Task 2",
							   semSTK2, 128, (Void*)0,
							   MLSOSAL_PRIO_OTHER_TASK_TEST);
	if(retVal != MLS_SUCCESS)
		return MLS_ERROR;

	retVal = mlsOsalTaskCreate(&semTask3, mlsSemTask3, "Sem Task 3",
							   semSTK3, 128, (Void*)0,
							   MLSOSAL_PRIO_OTHER_TASK_TEST);
	if(retVal != MLS_SUCCESS)
		return MLS_ERROR;

	retVal = mlsOsalTaskCreate(&semTask4, mlsSemTask4, "Sem Task 4",
							   semSTK4, 128, (Void*)0,
							   MLSOSAL_PRIO_OTHER_TASK_TEST);
	if(retVal != MLS_SUCCESS)
		return MLS_ERROR;

	for(index = 0; index < (MLSOSAL_TEST_TIMEOUT/MLSOSAL_TEST_TIME_CHECK); index++)
	{
		if(gSemCounter1 > 2 && gSemCounter2 > 2 && gSemCounter3 > 2)
		{
			mlsOsalTaskDelete(&semTask1);
			mlsOsalTaskDelete(&semTask2);
			mlsOsalTaskDelete(&semTask3);
			mlsOsalTaskDelete(&semTask4);
			return MLS_SUCCESS;
		}

		mlsOsalDelayMs(MLSOSAL_TEST_TIME_CHECK);
	}
	mlsOsalTaskDelete(&semTask1);
	mlsOsalTaskDelete(&semTask2);
	mlsOsalTaskDelete(&semTask3);
	mlsOsalTaskDelete(&semTask4);

	return MLS_ERROR;
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static mlsErrorCode_t mlsDeleteSem(Void)
{
	return mlsOsalSemDelete(&mySem);
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
Void mlsOsalTestSemCreate(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsCreateSem());
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
Void mlsOsalTestSemIsWorked(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsCheckSemIsWorked());
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
Void mlsOsalTestSemDel(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsDeleteSem());
}

#endif//MLS_OSAL_TEST
/************************************************************************************************************
 * _______________________________________________END FILE__________________________________________________
 */
