/*
 * mlsOsalTeskTask.c
 *
 *  Created on: Dec 7, 2015
 *      Author: root
 */

#include "mlsOsalTest.h"

#if(MLS_OSAL_TEST)
#include "../../Unity/inc/unity.h"

#define MLSOSAL_STACK_SIZE			128
#define MLSOSAL_SELF_STACK_SIZE		512

static mlsTaskHandle_t taskHandler, selfTaskHandler;

static UInt32 stackbuf[MLSOSAL_STACK_SIZE];
static UInt32 selfStackBuf[MLSOSAL_SELF_STACK_SIZE];

static Bool gTaskRunning = False;
static Bool gSelfTaskRunning = False;

/*********************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static Void mlsTaskEntryTest(Void *p_arg)
{
	MLS_UNUSED_PARAMETER(p_arg);

	while(1)
	{
		if(!gTaskRunning)
		{
			gTaskRunning = True;
		}
		mlsOsalDelayMs(500);
	}
}

/*********************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static Void mlsSelfTaskEntryTest(Void *p_arg)
{
	MLS_UNUSED_PARAMETER(p_arg);

	while(1)
	{
		if(!gSelfTaskRunning)
		{
			gSelfTaskRunning = True;
		}
		mlsOsalDelayMs(100);
	}
}

/*********************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static mlsErrorCode_t mlsCreateTask(Void)
{
	return ( mlsOsalTaskCreate(&taskHandler,
							   mlsTaskEntryTest,
							   "Task test",
							   stackbuf,
							   MLSOSAL_STACK_SIZE,
							   (Void*)0,
							   MLSOSAL_PRIO_OTHER_TASK_TEST));
}

/*********************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static mlsErrorCode_t mlsCreateTaskSelfManageStack(Void)
{
	return ( mlsOsalTaskCreate(
							   &selfTaskHandler,
							   mlsSelfTaskEntryTest,
							   "Self Task",
							   selfStackBuf,
							   MLSOSAL_SELF_STACK_SIZE,
							   (Void*)0,
							   MLSOSAL_PRIO_OTHER_TASK_TEST));
}

/*********************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static mlsErrorCode_t mlsDeleteTask(Void)
{
	return (mlsOsalTaskDelete(&taskHandler));
}

/*********************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static mlsErrorCode_t mlsDeleteSelfTask(Void)
{
	return (mlsOsalTaskDelete(&selfTaskHandler));
}

/*********************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static mlsErrorCode_t mlsCheckTaskIsRunning(Void)
{
	UInt8 index = 0;
	for(index = 0; index < (MLSOSAL_TEST_TIMEOUT/MLSOSAL_TEST_TIME_CHECK); index++)
	{
		if(gTaskRunning)
		{
			return MLS_SUCCESS;
		}

		mlsOsalDelayMs(MLSOSAL_TEST_TIME_CHECK);
	}
	return MLS_ERROR;
}

/*********************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static mlsErrorCode_t mlsCheckSelfTaskIsRunning(Void)
{
	UInt8 index = 0;
	for(index = 0; index < (MLSOSAL_TEST_TIMEOUT/MLSOSAL_TEST_TIME_CHECK); index++)
	{
		if(gSelfTaskRunning)
		{
			return MLS_SUCCESS;
		}

		mlsOsalDelayMs(MLSOSAL_TEST_TIME_CHECK);
	}
	return MLS_ERROR;
}

/*********************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsOsalTestCreateTask(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsCreateTask());
}

/*********************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsOsalTestCreateSelfTask(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsCreateTaskSelfManageStack());
}

/*********************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsOsalTestDeleteTask(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsDeleteTask());
}

/*********************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsOsalTestDeleteSelfTask(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsDeleteSelfTask());
}

/*********************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsOsalTestCheckTaskIsRunning(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsCheckTaskIsRunning());
}

/*********************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsOsalTestCheckSelfTaskIsRunning(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsCheckSelfTaskIsRunning());
}

#endif//MLS_OSAL_TEST
/**********************************************************************************************************
 * _______________________________________________END FILE________________________________________________
 */
