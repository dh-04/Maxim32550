/*
 * mlsOsalTest.h
 *
 *  Created on: Dec 7, 2015
 *      Author: root
 */

#ifndef _MLSOSALTEST_H_
#define _MLSOSALTEST_H_

#include "../inc/mlsOsal.h"

#if(MLS_OSAL_TEST)

#ifdef MLS_OSAL_FREE_RTOS
#define MLSOSAL_PRIO_MAIN_TASK_TEST		5
#define MLSOSAL_PRIO_OTHER_TASK_TEST	3
#define MLSOSAL_PRIO_PREEMP_HIGH		7
#define MLSOSAL_PRIO_PREEMP_LOW			3
#define MLSOSAL_PRIO_ROUND_ROBIN		3
#define MLSOSAL_PRIO_TEST_CHANGE_PRIO_1		MLSOSAL_PRIO_MAIN_TASK_TEST-1
#define MLSOSAL_PRIO_TEST_CHANGE_PRIO_2		MLSOSAL_PRIO_MAIN_TASK_TEST-2
#define MLSOSAL_PRIO_TO_CHANGE				MLSOSAL_PRIO_TEST_CHANGE_PRIO_2-1
#endif

#ifdef MLS_OSAL_UCOS_III
#define MLSOSAL_PRIO_MAIN_TASK_TEST			5
#define MLSOSAL_PRIO_OTHER_TASK_TEST		3
#define MLSOSAL_PRIO_PREEMP_HIGH			3
#define MLSOSAL_PRIO_PREEMP_LOW				7
#define MLSOSAL_PRIO_ROUND_ROBIN			7
#define MLSOSAL_PRIO_TEST_CHANGE_PRIO_1		MLSOSAL_PRIO_MAIN_TASK_TEST+1
#define MLSOSAL_PRIO_TEST_CHANGE_PRIO_2		MLSOSAL_PRIO_MAIN_TASK_TEST+2
#define MLSOSAL_PRIO_TO_CHANGE				MLSOSAL_PRIO_TEST_CHANGE_PRIO_2+1
#endif

#define MLSOSAL_TEST_TIMEOUT		10000
#define MLSOSAL_TEST_TIME_CHECK		100

typedef enum
{
	MLSOSAL_TEST_CASE_IDLE,

	MLSOSAL_TEST_CASE_TASK_CREATE,
	MLSOSAL_TEST_CASE_TASK_SELF_CREATE,
	MLSOSAL_TEST_CASE_TASK_IS_RUNNING,
	MLSOSAL_TEST_CASE_TASK_SELF_IS_RUNNING,
	MLSOSAL_TEST_CASE_TASK_DEL,
	MLSOSAL_TEST_CASE_TASK_SELF_DEL,

	MLSOSAL_TEST_CASE_MUTEX_CREATE,
	MLSOSAL_TEST_CASE_MUTEX_IS_WORKED,
	MLSOSAL_TEST_CASE_MUTEX_DEL,

	MLSOSAL_TEST_CASE_SEM_CREATE,
	MLSOSAL_TEST_CASE_SEM_IS_WORKED,
	MLSOSAL_TEST_CASE_SEM_DEL,

	MLSOSAL_TEST_CASE_EVENT_CREATE,
	MLSOSAL_TEST_CASE_EVENT_AUTO_SEND_AND_RECEIVE,
	MLSOSAL_TEST_CASE_EVENT_1_NOTIFY_FROM_EXT_INT,
	MLSOSAL_TEST_CASE_EVENT_2_NOTIFY_FROM_EXT_INT,
	MLSOSAL_TEST_CASE_EVENT_3_NOTIFY_FROM_EXT_INT,
	MLSOSAL_TEST_CASE_EVENT_ALL_NOTIFY_FROM_EXT_INT,
	MLSOSAL_TEST_CASE_EVENT_DEL,

	MLSOSAL_TEST_CASE_Q_CREATE,
	MLSOSAL_TEST_CASE_Q_AUTO_SEND_AND_RECEIVE_MSG,
	MLSOSAL_TEST_CASE_Q_SEND_START_FROM_EXT_INT,
	MLSOSAL_TEST_CASE_Q_SEND_RUNNING_FROM_EXT_INT,
	MLSOSAL_TEST_CASE_Q_SEND_STOP_FROM_EXT_INT,
	MLSOSAL_TEST_CASE_Q_DEL,

	MLSOSAL_TEST_CASE_PREEMP,

	MLSOSAL_TEST_CASE_ROUND_ROBIN,

	MLSOSAL_TEST_CASE_CHANGE_PRIO,
}mlsOsalUnityTestCase;

typedef struct mlsOsalMsgData_str
{
	UInt8 CRC;
	UInt8 len;
	UInt8 data[64];
}mlsOsalMsgData_t;

extern volatile mlsOsalUnityTestCase gOsalUnityTestCase;

Void mlsOsalTestUnit(Void* p_arg);

/******************************************************************************************************************/
Void mlsOsalTestCreateTask(Void);

Void mlsOsalTestCreateSelfTask(Void);

Void mlsOsalTestCheckTaskIsRunning(Void);

Void mlsOsalTestCheckSelfTaskIsRunning(Void);

Void mlsOsalTestDeleteTask(Void);

Void mlsOsalTestDeleteSelfTask(Void);

/******************************************************************************************************************/
Void mlsSendEvent1(Void);

Void mlsSendEvent2(Void);

Void mlsSendEvent3(Void);

Void mlsSendAllEvent(Void);

Void mlsOsalTestCreateEventFlags(Void);

Void mlsOsalTestEventsAutoSendAndReceive(Void);

Void mlsOsalTestWaitEvent1(Void);

Void mlsOsalTestWaitEvent2(Void);

Void mlsOsalTestWaitEvent3(Void);

Void mlsOsalTestWaitEventAll(Void);

Void mlsOsalTestDeleteEventFlags(Void);
/******************************************************************************************************************/
Void mlsOsalTestCreateMutex(Void);

Void mlsOsalTestMutexIsWorked(Void);

Void mlsOsalTestMutexDelete(Void);

/******************************************************************************************************************/
Void mlsOsalTestSemCreate(Void);

Void mlsOsalTestSemIsWorked(Void);

Void mlsOsalTestSemDel(Void);

/******************************************************************************************************************/
Void mlsOsalTestMsgQCreate(Void);

Void mlsOsalTestMsgQAutoSendAndReceive(Void);

Void mlsOsalTestMsgQWaitStart(Void);

Void mlsOsalTestMsgQWaitRunning(Void);

Void mlsOsalTestMsgQWaitStop(Void);

Void mlsOsalTestMsgQDelete(Void);

Void mlsSendMsgStart(Void);

Void mlsSendMsgStop(Void);

Void mlsSendMsgRunning(Void);

/******************************************************************************************************************/
Void mlsOsalTestPreemp(Void);

Void mlsOsalTestRoundRobin(Void);

Void mlsOsalTestChangePrio(Void);
Void mlsChangePrioTask1(Void);
/******************************************************************************************************************/
#endif	/*MLS_OSAL_TEST*/

#endif /*_MLSOSALTEST_H_ */
