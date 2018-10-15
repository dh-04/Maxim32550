/*
 * mlsOsalTest.c
 *
 *  Created on: Dec 7, 2015
 *      Author: root
 */

#include "mlsOsalTest.h"

#if(MLS_OSAL_TEST)

#include "../Unity/inc/unity.h"
#include "../mlsApp/inc/mlsLed.h"

volatile mlsOsalUnityTestCase gOsalUnityTestCase = MLSOSAL_TEST_CASE_IDLE;

/*********************************************************************************************************
 *@ Function	:
 *@ Brief		:
 *@ Parameter	:
 *@ Return Value:
 */
Void mlsOsalTestUnit(Void* p_arg)
{
	MLS_UNUSED_PARAMETER(p_arg);

	static int counter = 0;

	gOsalUnityTestCase = MLSOSAL_TEST_CASE_TASK_CREATE;
//	gOsalUnityTestCase = MLSOSAL_TEST_CASE_CHANGE_PRIO;

	while(1)
	{
		switch(gOsalUnityTestCase)
		{
		case MLSOSAL_TEST_CASE_TASK_CREATE:
			UnityBegin("mlsOsalTestTask.c");
			RUN_TEST(mlsOsalTestCreateTask, 153);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_TASK_SELF_CREATE;
			break;

		case MLSOSAL_TEST_CASE_TASK_SELF_CREATE:
			RUN_TEST(mlsOsalTestCreateSelfTask, 164);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_TASK_IS_RUNNING;
			break;

		case MLSOSAL_TEST_CASE_TASK_IS_RUNNING:
			RUN_TEST(mlsOsalTestCheckTaskIsRunning, 197);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_TASK_SELF_IS_RUNNING;
			break;

		case MLSOSAL_TEST_CASE_TASK_SELF_IS_RUNNING:
			RUN_TEST(mlsOsalTestCheckSelfTaskIsRunning, 208);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_TASK_DEL;
			break;

		case MLSOSAL_TEST_CASE_TASK_DEL:
			RUN_TEST(mlsOsalTestDeleteTask, 175);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_TASK_SELF_DEL;
			break;

		case MLSOSAL_TEST_CASE_TASK_SELF_DEL:
			RUN_TEST(mlsOsalTestDeleteSelfTask,	186);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_MUTEX_CREATE;
			break;

		case MLSOSAL_TEST_CASE_MUTEX_CREATE:
			UnitySetFileName("mlsOsalTestMutex.c");
			RUN_TEST(mlsOsalTestCreateMutex, 178);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_MUTEX_IS_WORKED;
			break;

		case MLSOSAL_TEST_CASE_MUTEX_IS_WORKED:
			RUN_TEST(mlsOsalTestMutexIsWorked, 189);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_MUTEX_DEL;
			break;

		case MLSOSAL_TEST_CASE_MUTEX_DEL:
			RUN_TEST(mlsOsalTestMutexDelete, 200);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_SEM_CREATE;
			break;

		case MLSOSAL_TEST_CASE_SEM_CREATE:
			UnitySetFileName("mlsOsalTestSem.c");
			RUN_TEST(mlsOsalTestSemCreate, 262);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_SEM_IS_WORKED;
			break;

		case MLSOSAL_TEST_CASE_SEM_IS_WORKED:
			RUN_TEST(mlsOsalTestSemIsWorked, 273);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_SEM_DEL;
			break;

		case MLSOSAL_TEST_CASE_SEM_DEL:
			RUN_TEST(mlsOsalTestSemDel, 284);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_EVENT_CREATE;
			break;

		case MLSOSAL_TEST_CASE_EVENT_CREATE:
			UnitySetFileName("mlsOsalTestEvent.c");
			RUN_TEST(mlsOsalTestCreateEventFlags, 266);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_EVENT_AUTO_SEND_AND_RECEIVE;
			break;

		case MLSOSAL_TEST_CASE_EVENT_AUTO_SEND_AND_RECEIVE:
			RUN_TEST(mlsOsalTestEventsAutoSendAndReceive, 277);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_EVENT_1_NOTIFY_FROM_EXT_INT;
			break;

		case MLSOSAL_TEST_CASE_EVENT_1_NOTIFY_FROM_EXT_INT:
			RUN_TEST(mlsOsalTestWaitEvent1,	288);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_EVENT_2_NOTIFY_FROM_EXT_INT;
			break;

		case MLSOSAL_TEST_CASE_EVENT_2_NOTIFY_FROM_EXT_INT:
			RUN_TEST(mlsOsalTestWaitEvent2,	302);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_EVENT_3_NOTIFY_FROM_EXT_INT;
			break;

		case MLSOSAL_TEST_CASE_EVENT_3_NOTIFY_FROM_EXT_INT:
			RUN_TEST(mlsOsalTestWaitEvent3, 316);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_EVENT_ALL_NOTIFY_FROM_EXT_INT;
			break;

		case MLSOSAL_TEST_CASE_EVENT_ALL_NOTIFY_FROM_EXT_INT:
			RUN_TEST(mlsOsalTestWaitEventAll, 330);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_EVENT_DEL;
			break;

		case MLSOSAL_TEST_CASE_EVENT_DEL:
			RUN_TEST(mlsOsalTestDeleteEventFlags, 344);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_Q_CREATE;
			break;

		case MLSOSAL_TEST_CASE_Q_CREATE:
			UnitySetFileName("mlsOsalTestMsgQueue.c");
			RUN_TEST(mlsOsalTestMsgQCreate, 311);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_Q_AUTO_SEND_AND_RECEIVE_MSG;
			break;

		case MLSOSAL_TEST_CASE_Q_AUTO_SEND_AND_RECEIVE_MSG:
			RUN_TEST(mlsOsalTestMsgQAutoSendAndReceive, 322);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_Q_SEND_START_FROM_EXT_INT;
			break;

		case MLSOSAL_TEST_CASE_Q_SEND_START_FROM_EXT_INT:
			RUN_TEST(mlsOsalTestMsgQWaitStart, 332);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_Q_SEND_RUNNING_FROM_EXT_INT;
			break;

		case MLSOSAL_TEST_CASE_Q_SEND_RUNNING_FROM_EXT_INT:
			RUN_TEST(mlsOsalTestMsgQWaitRunning, 346);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_Q_SEND_STOP_FROM_EXT_INT;
			break;

		case MLSOSAL_TEST_CASE_Q_SEND_STOP_FROM_EXT_INT:
			RUN_TEST(mlsOsalTestMsgQWaitStop, 360);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_Q_DEL;
			break;

		case MLSOSAL_TEST_CASE_Q_DEL:
			RUN_TEST(mlsOsalTestMsgQDelete, 374);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_PREEMP;
			break;

		case MLSOSAL_TEST_CASE_PREEMP:
			UnitySetFileName("mlsOsalTestPreemp.c");
			RUN_TEST(mlsOsalTestPreemp, 114);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_ROUND_ROBIN;
			break;

		case MLSOSAL_TEST_CASE_ROUND_ROBIN:
			UnitySetFileName("mlsOsalTestRoundRobin.c");
			RUN_TEST(mlsOsalTestRoundRobin, 106);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_CHANGE_PRIO;
			break;

		case MLSOSAL_TEST_CASE_CHANGE_PRIO:
			UnitySetFileName("mlsOsalTestTaskChangePrio.c");
			RUN_TEST(mlsOsalTestChangePrio, 108);
			gOsalUnityTestCase = MLSOSAL_TEST_CASE_IDLE;
			UnityEnd();
			break;

		default:
			break;
		}

		counter++;
		if(counter%2)
		{
			mlsLedOff();
		}
		else
		{
			mlsLedOn();
		}

		mlsOsalDelayMs(500);
	}
}
#endif //MLS_OSAL_TEST
/*********************************************************************************************************
 * _________________________________________________END FILE_____________________________________________
 */
