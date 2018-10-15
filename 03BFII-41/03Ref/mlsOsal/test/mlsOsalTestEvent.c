/*
 * mlsOsalTestEvent.c
 *
 *  Created on: Dec 7, 2015
 *      Author: root
 */

#include "mlsOsalTest.h"

#if(MLS_OSAL_TEST)

#include "../../Unity/inc/unity.h"
#include "../mlsDebug/inc/printf_lite.h"

#define EVENT_INIT	 0x00
#define EVENT_1		(0x01 << 0)
#define EVENT_2		(0x01 << 1)
#define EVENT_3		(0x01 << 2)

static mlsEventGroupHandle_t	eventFlags;
static mlsTaskHandle_t	taskSend, taskReceive;
static UInt32 taskSendSTK[128];
static UInt32 taskReceiveSTK[128];

static Bool gReceiveEvent1 		= False;
static Bool gReceiveEvent2 		= False;
static Bool gReceiveEvent3 		= False;
static Bool gReceiveAllEvent 	= False;

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static mlsErrorCode_t mlsCreateEventFlags(Void)
{
	return (mlsOsalEventGroupCreate(&eventFlags, "Event Flags", EVENT_INIT));
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static mlsErrorCode_t mlsWaitingForEvent1(Void)
{
	mlsEventBit_t	events;

	events = mlsOsalEventGroupWaitBits(&eventFlags,
									   EVENT_1,
									   10000,
									   MLSOSAL_OPT_EVENT_WAIT_ALL|
									   MLSOSAL_OPT_EVENT_WAIT_CLR_ON_EXIT);
	if(events != EVENT_1)
	{
		return MLS_ERROR;
	}
	return MLS_SUCCESS;
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static mlsErrorCode_t mlsWaitingForEvent2(Void)
{
	mlsEventBit_t	events;

	events = mlsOsalEventGroupWaitBits(&eventFlags,
									   EVENT_2,
									   10000,
									   MLSOSAL_OPT_EVENT_WAIT_ALL|
									   MLSOSAL_OPT_EVENT_WAIT_CLR_ON_EXIT);
	if(events != EVENT_2)
	{
		return MLS_ERROR;
	}

	return MLS_SUCCESS;
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static mlsErrorCode_t mlsWaitingForEvent3(Void)
{
	mlsEventBit_t	events;
	events = mlsOsalEventGroupWaitBits(&eventFlags,
									   EVENT_3,
									   10000,
									   MLSOSAL_OPT_EVENT_WAIT_ALL|
									   MLSOSAL_OPT_EVENT_WAIT_CLR_ON_EXIT);
	if(events != EVENT_3)
	{
		return MLS_ERROR;
	}

	return MLS_SUCCESS;
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static mlsErrorCode_t mlsWaitingForEventAll(Void)
{
	mlsEventBit_t	events;

	events = mlsOsalEventGroupWaitBits(&eventFlags,
									   EVENT_1|EVENT_2|EVENT_3,
									   10000,
									   MLSOSAL_OPT_EVENT_WAIT_ALL|
									   MLSOSAL_OPT_EVENT_WAIT_CLR_ON_EXIT);

	if(events != (EVENT_1|EVENT_2|EVENT_3))
	{
		return MLS_ERROR;
	}

	return MLS_SUCCESS;
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static mlsErrorCode_t mlsDeleteEventFlags(Void)
{
	return (mlsOsalEventGroupDelete(&eventFlags));
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static Void mlsTaskSend(Void* p_arg)
{
	static int count = 0;
	MLS_UNUSED_PARAMETER(p_arg);

	while(1)
	{
		count++;
		switch(count)
		{
		case 1:	mlsSendEvent1();	break;

		case 2:	mlsSendEvent2();	break;

		case 3:	mlsSendEvent3();	break;

		case 4:	mlsSendAllEvent();	break;

		default:
			break;
		}
		mlsOsalDelayMs(250);
	}
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static Void mlsTaskReceive(Void* p_arg)
{
	mlsEventBit_t	events;
	MLS_UNUSED_PARAMETER(p_arg);

	gReceiveEvent1 		= False;
	gReceiveEvent2 		= False;
	gReceiveEvent3 		= False;
	gReceiveAllEvent 	= False;

	while(1)
	{
		events = mlsOsalEventGroupWaitBits(&eventFlags,
										   EVENT_1|EVENT_2|EVENT_3,
										   MLS_OSAL_MAX_DELAY,
										   MLSOSAL_OPT_EVENT_WAIT_ANY|
										   MLSOSAL_OPT_EVENT_WAIT_CLR_ON_EXIT);

		if(events == EVENT_1) gReceiveEvent1 = True;
		else if(events == EVENT_2) gReceiveEvent2 = True;
		else if(events == EVENT_3) gReceiveEvent3 = True;
		else if(events == (EVENT_1|EVENT_2|EVENT_3)) gReceiveAllEvent = True;

		mlsOsalDelayMs(100);
	}
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
static mlsErrorCode_t mlsTestAutoSendAndReceiveEvent(Void)
{
	mlsErrorCode_t retVal;
	UInt8 index = 0;

	retVal = mlsOsalTaskCreate(&taskSend,
							   mlsTaskSend,
							   "Task send",
							   taskSendSTK,
							   128,
							   (Void*)0,
							   MLSOSAL_PRIO_OTHER_TASK_TEST);
	if(retVal != MLS_SUCCESS)
	{
		return retVal;
	}

	retVal = mlsOsalTaskCreate(&taskReceive,
							   mlsTaskReceive,
							   "Task receive",
							   taskReceiveSTK,
							   128,
							   (Void*)0,
							   MLSOSAL_PRIO_OTHER_TASK_TEST);
	if(retVal != MLS_SUCCESS)
	{
		return retVal;
	}

	for(index = 0; index < (MLSOSAL_TEST_TIMEOUT/MLSOSAL_TEST_TIME_CHECK); index++)
	{
		if(gReceiveAllEvent && gReceiveEvent1 && gReceiveEvent2 && gReceiveEvent3)
		{
			mlsOsalTaskDelete(&taskSend);
			mlsOsalTaskDelete(&taskReceive);
			return MLS_SUCCESS;
		}
		mlsOsalDelayMs(MLSOSAL_TEST_TIME_CHECK);
	}

	mlsOsalTaskDelete(&taskSend);
	mlsOsalTaskDelete(&taskReceive);

	return MLS_ERROR;
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsOsalTestCreateEventFlags(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsCreateEventFlags());
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsOsalTestEventsAutoSendAndReceive(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsTestAutoSendAndReceiveEvent());
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsOsalTestWaitEvent1(Void)
{
	lite_printf(DBG_PRINT_LEVEL_DEBUG_MASK, "\r\nPRESS KEY (v) to notify event 1!\r\n");
	lite_printf(DBG_PRINT_LEVEL_DEBUG_MASK, "10s - Waiting for event 1 ...\r\n");

	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsWaitingForEvent1());
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsOsalTestWaitEvent2(Void)
{
	lite_printf(DBG_PRINT_LEVEL_DEBUG_MASK, "\r\nPRESS KEY (v) to notify event 2!\r\n");
	lite_printf(DBG_PRINT_LEVEL_DEBUG_MASK, "10s - Waiting for event 2 ...\r\n");

	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsWaitingForEvent2());
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsOsalTestWaitEvent3(Void)
{
	lite_printf(DBG_PRINT_LEVEL_DEBUG_MASK, "\r\nPRESS KEY (v) to notify event 3!\r\n");
	lite_printf(DBG_PRINT_LEVEL_DEBUG_MASK, "10s - Waiting for event 3 ...\r\n");

	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsWaitingForEvent3());
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsOsalTestWaitEventAll(Void)
{
	lite_printf(DBG_PRINT_LEVEL_DEBUG_MASK, "\r\nPRESS KEY (v) to notify all event!\r\n");
	lite_printf(DBG_PRINT_LEVEL_DEBUG_MASK, "10s - Waiting for all event ...\r\n");

	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsWaitingForEventAll());
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsOsalTestDeleteEventFlags(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsDeleteEventFlags());
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsSendEvent1(Void)
{
	mlsOsalEventGroupSetBit(&eventFlags, EVENT_1);
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsSendEvent2(Void)
{
	mlsOsalEventGroupSetBit(&eventFlags, EVENT_2);
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsSendEvent3(Void)
{
	mlsOsalEventGroupSetBit(&eventFlags, EVENT_3);
}

/********************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsSendAllEvent(Void)
{
	mlsOsalEventGroupSetBit(&eventFlags, EVENT_1|EVENT_2|EVENT_3);
}

#endif //MLS_OSAL_TEST
/********************************************************************************************************************
 * _____________________________________________________END FILE____________________________________________________
 */
