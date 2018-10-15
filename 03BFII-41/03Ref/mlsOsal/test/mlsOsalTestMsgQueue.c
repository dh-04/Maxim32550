/*
 * mlsOsalTestMsgQueue.c
 *
 *  Created on: Dec 7, 2015
 *      Author: root
 */

#include "mlsOsalTest.h"

#if(MLS_OSAL_TEST)
#include <string.h>

#include "../../Unity/inc/unity.h"
#include "../mlsDebug/inc/printf_lite.h"

static mlsQueueHandle_t myQ;

static mlsTaskHandle_t msgTaskSend, msgTaskReceive;
static UInt32 msgTaskSendSTK[128];
static UInt32 msgTaskReceiveSTK[128];

static UInt8 start[] 	= "Start  ";
static UInt8 run[]	 	= "Running";
static UInt8 stop[] 	= "Stop   ";

static Bool gReceiveMsgStart 	= False;
static Bool gReceiveMsgRunning 	= False;
static Bool gReceiveMsgStop 	= False;

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static mlsErrorCode_t mlsCreateMsgQueue(Void)
{
	return (mlsOsalQueueCreate(&myQ, 10, "My Message Queue", sizeof(mlsOsalMsgData_t)));
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static mlsErrorCode_t mlsDeleteMsgQueue(Void)
{
	return (mlsOsalQueueDel(&myQ));
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static mlsErrorCode_t mlsWaitingForMsgStart(Void)
{
	mlsOsalMsgData_t data;
	UInt16 msgSize;

	mlsOsalQueueDequeue(&myQ,
						(Void*)&data,
						&msgSize,
						MLSOSAL_TEST_TIMEOUT);

	if(memcmp(&data.data[0], start, sizeof(start)) != 0)
	{
		return MLS_ERROR;
	}

	return MLS_SUCCESS;
}


/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static mlsErrorCode_t mlsWaitingForMsgRunning(Void)
{
	mlsOsalMsgData_t data;
	UInt16 msgSize;

	mlsOsalQueueDequeue(&myQ,
						(Void*)&data,
						&msgSize,
						MLSOSAL_TEST_TIMEOUT);

	if(memcmp(&data.data[0], run, sizeof(run)) != 0)
	{
		return MLS_ERROR;
	}

	return MLS_SUCCESS;
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static mlsErrorCode_t mlsWaitingForMsgStop(Void)
{
	mlsOsalMsgData_t data;
	UInt16 msgSize;

	mlsOsalQueueDequeue(&myQ,
						(Void*)&data,
						&msgSize,
						MLSOSAL_TEST_TIMEOUT);

	if(memcmp(&data.data[0], stop, sizeof(stop)) != 0)
	{
		return MLS_ERROR;
	}

	return MLS_SUCCESS;
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static Void mlsMsgTaskReceive(Void* p_arg)
{
	mlsOsalMsgData_t data;
	UInt16 msgSize;
	MLS_UNUSED_PARAMETER(p_arg);

	gReceiveMsgStart 	= False;
	gReceiveMsgRunning 	= False;
	gReceiveMsgStop 	= False;

	while(1)
	{
		mlsOsalQueueDequeue(&myQ, (Void*)&data, &msgSize, MLS_OSAL_MAX_DELAY);

		if(memcmp(&data.data[0], start, sizeof(start)) == 0) 	gReceiveMsgStart 	= True;
		else if(memcmp(&data.data[0], run, sizeof(run)) == 0) 	gReceiveMsgRunning 	= True;
		else if(memcmp(&data.data[0], stop, sizeof(stop)) == 0)	gReceiveMsgStop 	= True;

		mlsOsalDelayMs(100);
	}
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static Void mlsMsgTaskSend(Void* p_arg)
{
	static int count = 0;
	MLS_UNUSED_PARAMETER(p_arg);

	mlsOsalMsgData_t data;

	while(1)
	{
		count++;
		switch(count)
		{
		case 1:
			data.CRC = 0xff;
			data.len = sizeof(start);
			memcpy(&data.data[0], start, sizeof(start));
			mlsOsalQueueEnqueue(&myQ, &data, data.len+2, MLS_OSAL_MAX_DELAY);
			break;

		case 2:
			data.CRC = 0xf0;
			data.len = sizeof(run);
			memcpy(&data.data[0], run, sizeof(run));
			mlsOsalQueueEnqueue(&myQ, &data, data.len+2, MLS_OSAL_MAX_DELAY);
			break;

		case 3:
			data.CRC = 0x0f;
			data.len = sizeof(stop);
			memcpy(&data.data[0], stop, sizeof(stop));
			mlsOsalQueueEnqueue(&myQ, &data, data.len+2, MLS_OSAL_MAX_DELAY);
			break;

		default:
			break;
		}

		mlsOsalDelayMs(250);
	}
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
static mlsErrorCode_t mlsTestQAutoSendAndReceive(Void)
{
	mlsErrorCode_t retVal;
	UInt8 index = 0;

	retVal = mlsOsalTaskCreate(&msgTaskReceive,
							   mlsMsgTaskReceive,
							   "Task Receive",
							   msgTaskReceiveSTK,
							   128,
							   (Void*)0,
							   MLSOSAL_PRIO_OTHER_TASK_TEST);
	if(retVal != MLS_SUCCESS)
	{
		return MLS_ERROR;
	}

	retVal = mlsOsalTaskCreate(&msgTaskSend,
							   mlsMsgTaskSend,
							   "Task Send",
							   msgTaskSendSTK,
							   128,
							   (Void*)0,
							   MLSOSAL_PRIO_OTHER_TASK_TEST);
	if(retVal != MLS_SUCCESS)
	{
		return MLS_ERROR;
	}

	for(index = 0; index < (MLSOSAL_TEST_TIMEOUT/MLSOSAL_TEST_TIME_CHECK); index++)
	{
		if(gReceiveMsgStart && gReceiveMsgRunning && gReceiveMsgStop)
		{
			mlsOsalTaskDelete(&msgTaskReceive);
			mlsOsalTaskDelete(&msgTaskSend);
			return MLS_SUCCESS;
		}
		mlsOsalDelayMs(MLSOSAL_TEST_TIME_CHECK);
	}

	mlsOsalTaskDelete(&msgTaskReceive);
	mlsOsalTaskDelete(&msgTaskSend);

	return MLS_ERROR;
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
Void mlsSendMsgStart(Void)
{
	mlsOsalMsgData_t data;

	data.CRC = 0xff;
	data.len = sizeof(start);
	memcpy(&data.data[0], start, sizeof(start));

	mlsOsalQueueEnqueue(&myQ,
						&data,
						data.len+2,
						MLS_OSAL_MAX_DELAY);
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
Void mlsSendMsgStop(Void)
{
	mlsOsalMsgData_t data;

	data.CRC = 0xff;
	data.len = sizeof(stop);
	memcpy(&data.data[0], stop, sizeof(stop));
	mlsOsalQueueEnqueue(&myQ,
						&data,
						data.len+2,
						MLS_OSAL_MAX_DELAY);
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
Void mlsSendMsgRunning(Void)
{
	mlsOsalMsgData_t data;

	data.CRC = 0xff;
	data.len = sizeof(run);
	memcpy(&data.data[0], run, sizeof(run));
	mlsOsalQueueEnqueue(&myQ,
						&data,
						data.len+2,
						MLS_OSAL_MAX_DELAY);
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
Void mlsOsalTestMsgQCreate(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsCreateMsgQueue());
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
Void mlsOsalTestMsgQAutoSendAndReceive(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsTestQAutoSendAndReceive());
}
/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
Void mlsOsalTestMsgQWaitStart(Void)
{
	lite_printf(DBG_PRINT_LEVEL_DEBUG_MASK, "\r\nPRESS KEY (v) to send Start Message!\r\n");
	lite_printf(DBG_PRINT_LEVEL_DEBUG_MASK, "10s - Waiting for Start Message ...\r\n");

	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsWaitingForMsgStart());
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
Void mlsOsalTestMsgQWaitRunning(Void)
{
	lite_printf(DBG_PRINT_LEVEL_DEBUG_MASK, "\r\nPRESS KEY (v) to send Running Message!\r\n");
	lite_printf(DBG_PRINT_LEVEL_DEBUG_MASK, "10s - Waiting for Running Message ...\r\n");

	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsWaitingForMsgRunning());
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
Void mlsOsalTestMsgQWaitStop(Void)
{
	lite_printf(DBG_PRINT_LEVEL_DEBUG_MASK, "\r\nPRESS KEY (v) to send Stop Message!\r\n");
	lite_printf(DBG_PRINT_LEVEL_DEBUG_MASK, "10s - Waiting for Stop Message ...\r\n");

	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsWaitingForMsgStop());
}

/************************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return value:
 */
Void mlsOsalTestMsgQDelete(Void)
{
	TEST_ASSERT_EQUAL(MLS_SUCCESS, mlsDeleteMsgQueue());
}

#endif //MLS_OSAL_TEST
/************************************************************************************************************
 * __________________________________________________END FILE_______________________________________________
 */
