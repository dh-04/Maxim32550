
/**
 * \addtogroup mlsOsal
 * @{
 */

/**
 * @file mlsOsal.c
 * @brief Osal Component Implementation
 *
 * @date
 * @author
 */

/********** Include section ***************************************************/
#include "../inc/mlsOsal.h"

#include <string.h>
#include <mml_gcr.h>

#ifdef MLS_OSAL_UCOS_III
#include "alloca.h"
#endif

/********** Local Constant and compile switch definition section **************/

/********** Local Type definition section *************************************/

/********** Local Macro definition section ************************************/

/********** Local (static) variable definition section ************************/
#ifdef MLS_OSAL_UCOS_III
CPU_SR cpuSR;
#endif
static Bool gIsOsSchedulerStarted = False;

/********** Local (static) function declaration section ***********************/
static void __attribute__ ((noinline))  __attribute__((optimize("-O0")))
		mlsOsalDelayMsStart(unsigned int ms);

/********** Local function definition section *********************************/

/********** Global function definition section ********************************/

#pragma GCC optimize("O0")
/******************************************************************************
 * mlsOsalDelayMsStart
 */
static void __attribute__ ((noinline))  __attribute__((optimize("-O0")))
		mlsOsalDelayMsStart(unsigned int ms)
{
     int i = ms*5000;
     unsigned int systemfreq = 0;
     mml_get_system_frequency(&systemfreq);

     i /= (MML_SYSTEM_FREQUENCY_108_MHZ/systemfreq);

     for (; i > 0 ; i--)
          __asm volatile ("nop\n");
}
#pragma GCC reset_options

/*****************************************************************************************
 ________________________________________OSAL Initialize__________________________________
 *****************************************************************************************/
/*****************************************************************************************
 * mlsOsalInit
 */
mlsErrorCode_t mlsOsalInit(Void)
{
	mlsErrorCode_t retVal = MLS_SUCCESS;

#ifdef MLS_OSAL_FREE_RTOS
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR	os_err;

	CPU_Init();

	OSInit(&os_err);

	OS_CSP_TickInit();

#if(OS_CFG_STAT_TASK_EN)
//	OSStatTaskCPUUsageInit(&os_err);
#endif

	if(os_err != OS_ERR_NONE)
	{
		retVal = MLS_ERROR;
	}
#endif

	return retVal;
}

/*****************************************************************************************
 _________________________________________Task Schedule___________________________________
 *****************************************************************************************/
/*****************************************************************************************
 * mlsOsalDeInit
 */
Void mlsOsalTaskSchedule (Void)
{
	gIsOsSchedulerStarted = True;

#ifdef MLS_OSAL_FREE_RTOS
	vTaskStartScheduler();
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	OSStart((OS_ERR*)&err);
#endif

}

/*****************************************************************************************
 * mlsOsalSchedRoundRobinCfg
 */
mlsErrorCode_t mlsOsalSchedRoundRobinCfg(UInt8 bEnable, mlsTickType_t timeQuanta)
{
	mlsErrorCode_t retVal = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	MLS_UNUSED_PARAMETER(bEnable);
	MLS_UNUSED_PARAMETER(timeQuanta);
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	OSSchedRoundRobinCfg(
					(CPU_BOOLEAN)bEnable,
					(OS_TICK)timeQuanta,
					(OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retVal = MLS_ERROR;
	}
#endif
	return retVal;
}

/*****************************************************************************************
 * mlsOsalSchedRoundRobinYield
 */
mlsErrorCode_t mlsOsalSchedRoundRobinYield(Void)
{
	mlsErrorCode_t retVal = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;

	OSSchedRoundRobinYield(&err);

	if(err != OS_ERR_NONE)
	{
		retVal = MLS_ERROR;
	}

#endif

	return retVal;
}

/*****************************************************************************************
 ________________________________________Task Management__________________________________
 *****************************************************************************************/
/*****************************************************************************************
 * mlsOsalTaskCreate
 */
mlsErrorCode_t mlsOsalTaskCreate(
							mlsTaskHandle_t*	pTaskHandle,
							mlsTaskEntry_t 		entry,
							const Char* 		pTaskName,
							UInt32*				pStackBuf,
							UInt16 				wStacksize,
							Void* 				pParameter,
							Int16 				wPriority)
{
	mlsErrorCode_t retVal = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	mlsOsalBaseType_t status;
	MLS_UNUSED_PARAMETER(pStackBuf);

	status = xTaskCreate(
					(pdTASK_CODE)entry,
					pTaskName,
					wStacksize,
					pParameter,
					wPriority,
					pTaskHandle);

	if(status != pdPASS)
	{
		retVal = MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;

	OSTaskCreate(
			(OS_TCB*)pTaskHandle,
			(CPU_CHAR*)pTaskName,
			(OS_TASK_PTR)entry,
			(Void*)pParameter,
			(OS_PRIO)wPriority,
			(CPU_STK*)pStackBuf,
			(CPU_STK_SIZE)(wStacksize/MLSOSAL_DIV_STACK_SIZE_LIMIT),
			(CPU_STK_SIZE)wStacksize,
			(OS_MSG_QTY)0,
			(OS_TICK)0,
			(Void*)0,
			(OS_OPT)MLSOSAL_OPT_TASK_CREATE,
			(OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retVal = MLS_ERROR;
	}

#endif
	return retVal;
}

/*****************************************************************************************
 * mlsOsalTaskCreateSelfManageStack
 */
mlsErrorCode_t mlsOsalTaskCreateSelfManageStack(
							mlsTaskHandle_t*	pTaskHandle,
							mlsTaskEntry_t 		entry,
							const Char* 		pTaskName,
							UInt32* 			pStackBuf,
							UInt16 				wStacksize,
							Void* 				pParameter,
							Int16 				wPriority)
{
	mlsErrorCode_t retVal = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	mlsOsalBaseType_t status;

	status = xTaskCreateMls(
					(pdTASK_CODE)entry,
					pTaskName,
					wStacksize,
					pParameter,
					wPriority,
					pStackBuf,
					pTaskHandle);

	if(status != pdPASS)
	{
		retVal = MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	OSTaskCreate(
			(OS_TCB*)pTaskHandle,
			(CPU_CHAR*)pTaskName,
			(OS_TASK_PTR)entry,
			(Void*)pParameter,
			(OS_PRIO)wPriority,
			(CPU_STK*)pStackBuf,
			(CPU_STK_SIZE)(wStacksize/MLSOSAL_DIV_STACK_SIZE_LIMIT),
			(CPU_STK_SIZE)wStacksize,
			(OS_MSG_QTY)0,
			(OS_TICK)0,
			(Void*)0,
			(OS_OPT)MLSOSAL_OPT_TASK_CREATE,
			(OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retVal = MLS_ERROR;
	}
#endif
	return retVal;
}

/*****************************************************************************************
 * mlsOsalTaskSuspend
 */
mlsErrorCode_t mlsOsalTaskSuspend(mlsTaskHandle_t* pTaskHandle)
{
	mlsErrorCode_t retval = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	vTaskSuspend( *pTaskHandle );

	if (!pTaskHandle)
	{
		retval = MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	OSTaskSuspend((OS_TCB *)pTaskHandle, &err);

	if(err != OS_ERR_NONE)
	{
		retval = MLS_ERROR;
	}
#endif

	return retval;
}

/*****************************************************************************************
 * mlsOsalTaskResume
 */
mlsErrorCode_t mlsOsalTaskResume(mlsTaskHandle_t* pTaskHandle)
{
	mlsErrorCode_t retVal = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	vTaskResume( *pTaskHandle );
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	OSTaskResume((OS_TCB *)pTaskHandle, &err);

	if(err != OS_ERR_NONE)
	{
		retVal = MLS_ERROR;
	}
#endif
	return retVal;
}

/*****************************************************************************************
 * mlsOsalTaskDelete
 */
mlsErrorCode_t mlsOsalTaskDelete(mlsTaskHandle_t* pTaskHandle)
{
	mlsErrorCode_t retVal = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	vTaskDelete(*pTaskHandle);
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	OSTaskDel((OS_TCB *)pTaskHandle, &err);

	if(err != OS_ERR_NONE)
	{
		retVal = MLS_ERROR;
	}
#endif

	return retVal;
}

/*****************************************************************************************
 * mlsOsalTaskChangePrio
 */
mlsErrorCode_t mlsOsalTaskChangePrio(mlsTaskHandle_t* pTaskHandle, Int16 wNewPrio)
{
	mlsErrorCode_t retVal = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	vTaskPrioritySet(*pTaskHandle, wNewPrio);
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	OSTaskChangePrio(pTaskHandle, wNewPrio, &err);

	if(err != OS_ERR_NONE)
	{
		retVal = MLS_ERROR;
	}
#endif

	return retVal;
}

/*****************************************************************************************
 ______________________________Mutual Exclusion Semaphores________________________________
 *****************************************************************************************/
/*****************************************************************************************
 * mlsOsalMutexCreate
 */
mlsErrorCode_t mlsOsalMutexCreate(mlsMutexHandle_t* pMutexHandle, const Char* pName)
{
	mlsErrorCode_t retVal = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS

	MLS_UNUSED_PARAMETER(pName);

	*pMutexHandle =  xSemaphoreCreateMutex();

	if(*pMutexHandle == NULL)
	{
		retVal = MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	OSMutexCreate((OS_MUTEX*)pMutexHandle, (CPU_CHAR*)pName, (OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retVal = MLS_ERROR;
	}
#endif
	return retVal;
}

/*****************************************************************************************
 * mlsOsalMutexLock
 */
mlsErrorCode_t mlsOsalMutexLock(mlsMutexHandle_t* pMutexHandle, UInt32 dwBlockTime)
{
	mlsErrorCode_t retval = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS

	if (xSemaphoreTakeRecursive( *pMutexHandle, dwBlockTime) != pdPASS )
	{
		retval = MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	OSMutexPend(
			(OS_MUTEX*)pMutexHandle,
			(OS_TICK)dwBlockTime,
			(OS_OPT) MLSOSAL_OPT_MUTEX_LOCK,
			(CPU_TS*)0, (OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retval = MLS_ERROR;
	}
#endif
	return retval;
}

/*****************************************************************************************
 * mlsOsalMutexUnlock
 */
mlsErrorCode_t mlsOsalMutexUnlock(mlsMutexHandle_t* pMutexHandle)
{
	mlsErrorCode_t retval = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	if (xSemaphoreGiveRecursive( *pMutexHandle ) != pdPASS )
	{
		retval = MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;

	OSMutexPost(
			(OS_MUTEX*)pMutexHandle,
			(OS_OPT)MLSOSAL_OPT_MUTEX_UNLOCK,
			(OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retval = MLS_ERROR;
	}
#endif
	return retval;
}

/*****************************************************************************************
 * mlsOsalMutexCheck
 */
mlsErrorCode_t mlsOsalMutexCheck(mlsMutexHandle_t* pMutexHandle)
{
	mlsErrorCode_t retval = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	if (xSemaphoreTakeRecursive( *pMutexHandle, 0 ) != pdPASS )
	{
		retval = MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;

	OSMutexPend(
			(OS_MUTEX*)pMutexHandle,
			(OS_TICK)0,
			(OS_OPT)MLSOSAL_OPT_MUTEX_CHECK,
			(CPU_TS*)0,
			(OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retval = MLS_ERROR;
	}
#endif
	return retval;
}

/*****************************************************************************************
 * mlsOsalMutexDelete
 */
mlsErrorCode_t mlsOsalMutexDelete(mlsMutexHandle_t* pMutexHandle)
{
	mlsErrorCode_t retVal = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	vSemaphoreDelete(*pMutexHandle);
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;

	OSMutexDel(
			(OS_MUTEX*)pMutexHandle,
			(OS_OPT) MLSOSAL_OPT_MUTEX_DELETE,
			(OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retVal = MLS_ERROR;
	}
#endif

	return retVal;
}

/*****************************************************************************************
 _________________________________________Semaphores______________________________________
 *****************************************************************************************/
/*****************************************************************************************
 * mlsOsalSemCreate
 */
mlsErrorCode_t mlsOsalSemCreate(
							mlsSemHandle_t* pSemHandle,
							const Char* pName,
							UInt32 dwMaxCount,
							UInt32 dwInitialCount)
{
	mlsErrorCode_t retVal = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	MLS_UNUSED_PARAMETER(pName);

	*pSemHandle = xSemaphoreCreateCounting( dwMaxCount, dwInitialCount );

	if(*pSemHandle == NULL)
	{
		retVal = MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	MLS_UNUSED_PARAMETER(dwInitialCount);

	OSSemCreate(
			(OS_SEM*)pSemHandle,
			(CPU_CHAR*)pName,
			(OS_SEM_CTR)dwMaxCount,
			(OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retVal = MLS_ERROR;
	}
#endif
	return retVal;
}

/*****************************************************************************************
 * mlsOsalSemGet
 */
mlsErrorCode_t mlsOsalSemGet(mlsSemHandle_t* pSemHandle, UInt32 dwTimeOut)
{
	mlsErrorCode_t retval = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	if (pdPASS != xSemaphoreTake( *pSemHandle, dwTimeOut ) )
	{
		retval = MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	OSSemPend(
			(OS_SEM*)pSemHandle,
			(OS_TICK)dwTimeOut,
			(OS_OPT)MLSOSAL_OPT_SEM_GET,
			(CPU_TS*)0,
			(OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retval = MLS_ERROR;
	}
#endif
	return retval;
}

/*****************************************************************************************
 * mlsOsalSemGet
 */
mlsErrorCode_t mlsOsalSemGetFromISR(mlsSemHandle_t* pSemHandle, mlsOsalBaseType_t *pHigherPrioTaskWoken)
{
	mlsErrorCode_t retval = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	if (pdPASS != xSemaphoreTakeFromISR( *pSemHandle, pHigherPrioTaskWoken))
	{
		retval = MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;

	MLS_UNUSED_PARAMETER(pHigherPrioTaskWoken);

	OSSemPend(
			(OS_SEM*)pSemHandle,
			(OS_TICK)0,
			(OS_OPT)MLSOSAL_OPT_SEM_GET,
			(CPU_TS*)0,
			(OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retval = MLS_ERROR;
	}
#endif
	return retval;
}

/*****************************************************************************************
 * mlsOsalSemSet
 */
mlsErrorCode_t mlsOsalSemSet(mlsSemHandle_t* pSemHandle)
{
	mlsErrorCode_t retval = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS

	if (pdPASS != xSemaphoreGive( *pSemHandle ))
	{
		retval = MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;

	OSSemPost(
			(OS_SEM*)pSemHandle,
			(OS_OPT)MLSOSAL_OPT_SEM_SET,
			(OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retval = MLS_ERROR;
	}
#endif
	return retval;
}

/*****************************************************************************************
 * mlsOsalSemSetFromISR
 */
mlsErrorCode_t mlsOsalSemSetFromISR(mlsSemHandle_t* pSemHandle, mlsOsalBaseType_t *pHigherPrioTaskWoken)
{
	mlsErrorCode_t retval = MLS_SUCCESS;

#ifdef MLS_OSAL_FREE_RTOS

	if (pdPASS != xSemaphoreGiveFromISR( *pSemHandle, pHigherPrioTaskWoken ))
	{
		retval = MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	MLS_UNUSED_PARAMETER(pHigherPrioTaskWoken);

	OSSemPost(
			(OS_SEM*)pSemHandle,
			(OS_OPT)MLSOSAL_OPT_SEM_SET,
			(OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retval = MLS_ERROR;
	}
#endif

	return retval;
}

/*****************************************************************************************
 * mlsOsalSemSetFromISR
 */
mlsErrorCode_t mlsOsalSemDelete(mlsSemHandle_t* pSemHandle)
{
	mlsErrorCode_t retVal = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	vSemaphoreDelete(*pSemHandle);
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;

	OSSemDel(
		(OS_SEM*)pSemHandle,
		(OS_OPT)MLSOSAL_OPT_SEM_DEL,
		(OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retVal = MLS_ERROR;
	}
#endif
	return retVal;
}

/*****************************************************************************************
 ________________________________________Event Flags______________________________________
 *****************************************************************************************/
/*****************************************************************************************
 * mlsOsalEventGroupCreate
 */
mlsErrorCode_t mlsOsalEventGroupCreate(
							mlsEventGroupHandle_t* pEventHandle,
							const Char* pName,
							UInt32 dwInitialEvent)
{
	mlsErrorCode_t retVal = MLS_SUCCESS;

#ifdef MLS_OSAL_FREE_RTOS
	MLS_UNUSED_PARAMETER(pName);

	*pEventHandle = xEventGroupCreate();

	if(*pEventHandle == NULL)
	{
		retVal = MLS_ERROR;
	}
	else
	{
		xEventGroupSetBits(*pEventHandle, dwInitialEvent);
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;

	OSFlagCreate(
			(OS_FLAG_GRP*)pEventHandle,
			(CPU_CHAR*)pName,
			(OS_FLAGS)dwInitialEvent,
			(OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retVal = MLS_ERROR;
	}
#endif
	return retVal;
}

/*****************************************************************************************
 * mlsOsalEventGroupSetBit
 */
mlsEventBit_t mlsOsalEventGroupSetBit(mlsEventGroupHandle_t* pEventHandle, const mlsEventBit_t eventBits)
{
#ifdef MLS_OSAL_FREE_RTOS
	return (mlsEventBit_t)(xEventGroupSetBits(*pEventHandle, eventBits));
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	return (mlsEventBit_t)(OSFlagPost(
								(OS_FLAG_GRP*)pEventHandle,
								(OS_FLAGS)eventBits,
								(OS_OPT)MLSOSAL_OPT_EVENT_SET,
								(OS_ERR*)&err));
#endif
}

/*****************************************************************************************
 * mlsOsalEventGroupSetBitFromISR
 */
mlsErrorCode_t mlsOsalEventGroupSetBitFromISR(
								mlsEventGroupHandle_t* pEventHandle,
								const mlsEventBit_t eventBits,
								mlsOsalBaseType_t* pHigherPrioTaskWoken)
{
	mlsErrorCode_t retVal = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS

	mlsOsalBaseType_t status;
	status = xEventGroupSetBitsFromISR(*pEventHandle, eventBits, pHigherPrioTaskWoken);

	if(status == pdFAIL)
	{
		retVal = MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	MLS_UNUSED_PARAMETER(pHigherPrioTaskWoken);

	OSFlagPost(
			(OS_FLAG_GRP*)pEventHandle,
			(OS_FLAGS)eventBits,
			(OS_OPT)MLSOSAL_OPT_EVENT_SET,
			(OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retVal = MLS_ERROR;
	}
#endif

	return retVal;
}

/*****************************************************************************************
 * mlsOsalEventGroupClearBit
 */
mlsEventBit_t mlsOsalEventGroupClearBit(mlsEventGroupHandle_t* pEventHandle, const mlsEventBit_t eventBits)
{
#ifdef MLS_OSAL_FREE_RTOS
	return (mlsEventBit_t)(xEventGroupClearBits(*pEventHandle, eventBits));
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	return (mlsEventBit_t)(OSFlagPost(
								(OS_FLAG_GRP*)pEventHandle,
								(OS_FLAGS)eventBits,
								(OS_OPT)MLSOSAL_OPT_EVENT_CLR,
								(OS_ERR*)&err));
#endif
}

/*****************************************************************************************
 * mlsOsalEventGroupClearBitFromISR
 */
mlsEventBit_t mlsOsalEventGroupClearBitFromISR(
								mlsEventGroupHandle_t* pEventHandle,
								const mlsEventBit_t eventBits)
{
#ifdef MLS_OSAL_FREE_RTOS
	return(xEventGroupClearBitsFromISR(*pEventHandle, eventBits));
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	return (mlsEventBit_t)(OSFlagPost(
								(OS_FLAG_GRP*)pEventHandle,
								(OS_FLAGS)eventBits,
								(OS_OPT)MLSOSAL_OPT_EVENT_CLR,
								(OS_ERR*)&err));
#endif
}

/*****************************************************************************************
 * mlsOsalEventGroupGetBit
 */
mlsEventBit_t mlsOsalEventGroupGetBit(mlsEventGroupHandle_t* pEventHandle)
{
#ifdef MLS_OSAL_FREE_RTOS
	return (mlsEventBit_t)(xEventGroupGetBits(*pEventHandle));
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	MLS_UNUSED_PARAMETER(pEventHandle);
	return (mlsEventBit_t)(OSFlagPendGetFlagsRdy((OS_ERR*)&err));
#endif
}

/*****************************************************************************************
 * mlsOsalEventGroupGetBitFromISR
 */
mlsEventBit_t mlsOsalEventGroupGetBitFromISR(mlsEventGroupHandle_t pEventHandle)
{
#ifdef MLS_OSAL_FREE_RTOS
	return (xEventGroupGetBitsFromISR(pEventHandle));
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	MLS_UNUSED_PARAMETER(pEventHandle);
	return (mlsEventBit_t)(OSFlagPendGetFlagsRdy((OS_ERR*)&err));
#endif
}

/*****************************************************************************************
 * mlsOsalEventGroupDelete
 */
mlsErrorCode_t mlsOsalEventGroupDelete(mlsEventGroupHandle_t* pEventHandle)
{
	mlsErrorCode_t retVal = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	vEventGroupDelete(*pEventHandle);
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;

	OSFlagDel(
			(OS_FLAG_GRP*)pEventHandle,
			(OS_OPT)MLSOSAL_OPT_EVENT_DEL,
			(OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retVal = MLS_ERROR;
	}
#endif
	return retVal;
}

/*****************************************************************************************
 * mlsOsalEventGroupWaitBits
 */
mlsEventBit_t mlsOsalEventGroupWaitBits(
							mlsEventGroupHandle_t* pEventHandle,
							const mlsEventBit_t eventBits,
							mlsTickType_t tickToWait,
							UInt16 wOption)
{
#ifdef MLS_OSAL_FREE_RTOS
	mlsOsalBaseType_t xClearOnExit;
	mlsOsalBaseType_t xWaitForAllBit ;

	if(wOption & MLSOSAL_OPT_EVENT_WAIT_CLR_ON_EXIT)
	{
		xClearOnExit = gTrue;
	}
	else
	{
		xClearOnExit = gFalse;
	}

	if(wOption & MLSOSAL_OPT_EVENT_WAIT_ALL)
	{
		xWaitForAllBit = gTrue;
	}
	else
	{
		xWaitForAllBit = gFalse;
	}

	return (xEventGroupWaitBits(
							*pEventHandle,
							eventBits,
							xClearOnExit,
							xWaitForAllBit,
							tickToWait));
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;

	return (mlsEventBit_t) (OSFlagPend(
								(OS_FLAG_GRP*)pEventHandle,
								(OS_FLAGS) eventBits,
								(OS_TICK)tickToWait,
								(OS_OPT)(wOption | MLSOSAL_OPT_WAIT_BLOCKING),
								(CPU_TS*)0,
								(OS_ERR*)&err));
#endif
}

/*****************************************************************************************
 ________________________________________Message Queue____________________________________
 *****************************************************************************************/
/*****************************************************************************************
 * mlsOsalQueueCreate
 */
mlsErrorCode_t mlsOsalQueueCreate(
							mlsQueueHandle_t* pQueueHandle,
							UInt32 dwQueueSize,
							const Char* pName,
							UInt16 wItemSize)
{
	mlsErrorCode_t retVal = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	MLS_UNUSED_PARAMETER(pName);

	*pQueueHandle = (mlsQueueHandle_t)xQueueCreate( dwQueueSize, wItemSize);

	if(*pQueueHandle == NULL)
	{
		retVal = MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	MLS_UNUSED_PARAMETER(dwQueueSize);

	OSQCreate((OS_Q*)pQueueHandle, (CPU_CHAR*)pName, (OS_MSG_QTY)wItemSize, (OS_ERR*)&err);

	if(err != MLS_SUCCESS)
	{
		retVal = MLS_ERROR;
	}
#endif
	return retVal;
}

/*****************************************************************************************
 * mlsOsalQueueEnqueue
 */
mlsErrorCode_t mlsOsalQueueEnqueue(
							mlsQueueHandle_t* pQueueHandle,
							Void* pData,
							UInt16 wMsgSize,
							UInt32 dwMaxBlockingTimeMs,
							Bool isFromISR)
{
	mlsErrorCode_t retval = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	UInt32 osRet;
	mlsOsalBaseType_t pxHigherPriorityTaskWoken;

	MLS_UNUSED_PARAMETER(wMsgSize);

	if(isFromISR)
	{
		osRet = xQueueSendFromISR(
							*pQueueHandle,
							pData,
							&pxHigherPriorityTaskWoken);
	}
	else
	{
		osRet = xQueueSend(
						*pQueueHandle,
						pData,
						(dwMaxBlockingTimeMs/portTICK_RATE_MS) );
	}

	if (pdTRUE != osRet)
	{
		retval = MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	MLS_UNUSED_PARAMETER(dwMaxBlockingTimeMs);
	MLS_UNUSED_PARAMETER(isFromISR);

	OSQPost(
		(OS_Q*) pQueueHandle,
		(Void*) pData,
		(OS_MSG_SIZE) wMsgSize,
		(OS_OPT) MLSOSAL_OPT_ENQUEUE,
		(OS_ERR*) &err);

	if(err != OS_ERR_NONE)
	{
		retval = MLS_ERROR;
	}
#endif
	return retval;
}

/*****************************************************************************************
 * mlsOsalQueueEnqueueToFront
 */
mlsErrorCode_t mlsOsalQueueEnqueueToFront(mlsQueueHandle_t* pQueueHandle,
										  Void* pData,
										  UInt16 wMsgSize,
										  UInt32 dwMaxBlockingTimeMs,
										  Bool isFromISR)
{
	mlsErrorCode_t retval = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	UInt32 osRet;
	mlsOsalBaseType_t pxHigherPriorityTaskWoken;

	MLS_UNUSED_PARAMETER(wMsgSize);

	if(isFromISR)
	{
		osRet = xQueueSendToFrontFromISR(
							*pQueueHandle,
							pData,
							&pxHigherPriorityTaskWoken);
	}
	else
	{
		osRet = xQueueSendToFront(
							*pQueueHandle,
							pData,
							(dwMaxBlockingTimeMs/portTICK_RATE_MS));
	}

	if (pdTRUE != osRet)
	{
		retval = MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	MLS_UNUSED_PARAMETER(dwMaxBlockingTimeMs);
	MLS_UNUSED_PARAMETER(isFromISR);

	OSQPost(
		(OS_Q*)pQueueHandle,
		(Void*) pData,
		(OS_MSG_SIZE) wMsgSize,
		(OS_OPT)MLSOSAL_OPT_ENQUEUE_TO_FRONT,
		(OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		retval = MLS_ERROR;
	}
#endif
	return retval;
}

/*****************************************************************************************
 * mlsOsalQueueDequeue
 */
mlsErrorCode_t mlsOsalQueueDequeue(
							mlsQueueHandle_t* pQueueHandle,
							Void* pData,
							UInt16* pMsgSize,
							UInt32 dwMaxBlockingTimeMs,
							Bool isFromISR)
{
	mlsErrorCode_t retval = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	UInt32 osRet;
	mlsOsalBaseType_t xHigherPriorityTaskWoken;

	MLS_UNUSED_PARAMETER(pMsgSize);

	if(isFromISR)
	{
		osRet = xQueueReceiveFromISR(
						*pQueueHandle,
						pData,
						&xHigherPriorityTaskWoken);
	}
	else
	{
		osRet = xQueueReceive(
						*pQueueHandle,
						pData,
						(dwMaxBlockingTimeMs/portTICK_RATE_MS));
	}

	if (pdTRUE != osRet)
	{
		retval = MLS_ERROR;
	}

#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	Void *pvdata;

	MLS_UNUSED_PARAMETER(isFromISR);

	pvdata =  OSQPend(
					(OS_Q*)pQueueHandle,
					(OS_TICK)dwMaxBlockingTimeMs,
					(OS_OPT)MLSOSAL_OPT_DEQUEUE,
					(OS_MSG_SIZE*)pMsgSize,
					(CPU_TS*)0,
					(OS_ERR*)&err);

	memcpy(pData, pvdata, *pMsgSize);

	if(err != OS_ERR_NONE)
	{
		retval = MLS_ERROR;
	}
#endif
	return retval;
}

/*****************************************************************************************
 * mlsOsalQueueNumberOfItemWaiting
 */
UInt32 mlsOsalQueueNumberOfItemWaiting(mlsQueueHandle_t* pQueueHandle)
{
#ifdef MLS_OSAL_FREE_RTOS
	return (UInt32)uxQueueMessagesWaiting(*pQueueHandle);
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	OS_MSG_QTY numMsg;

	numMsg = OSQFlush((OS_Q*)pQueueHandle, (OS_ERR*)&err);

	if(err != OS_ERR_NONE)
	{
		return 0;
	}
	else
	{
		return (UInt32)numMsg;
	}
#endif
}

/*****************************************************************************************
 * mlsOsalQueueDel
 */
mlsErrorCode_t mlsOsalQueueDel(mlsQueueHandle_t* pQueueHandle)
{
	mlsErrorCode_t retVal = MLS_SUCCESS;
#ifdef MLS_OSAL_FREE_RTOS
	vQueueDelete(*pQueueHandle);
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	OSQDel(pQueueHandle, (OS_OPT)MLSOSAL_OPT_QUEUE_DEL, &err);

	if(err != OS_ERR_NONE)
	{
		retVal = MLS_ERROR;
	}
#endif

	return retVal;
}

/*****************************************************************************************
 ___________________________________________Timer_________________________________________
 *****************************************************************************************/
/*****************************************************************************************
 * mlsOsalTmrCreate
 */
mlsErrorCode_t mlsOsalTmrCreate(
						mlsTmrHandle_t *pTmrHandle,
						const char* pName,
						UInt8 bAutoReload,
						void* const pTimeID,
						UInt32 dwDelay,
						UInt32 dwPeriod,
						mlsTmrCallbackFunc_t callback)
{
#ifdef MLS_OSAL_FREE_RTOS
	*pTmrHandle = xTimerCreate(
						pName,		/*Name*/
						dwPeriod,		/*Period*/
						bAutoReload,	/*Auto reload*/
						pTimeID,		/*ID*/
						callback);	/*Callback function*/

	if(*pTmrHandle == NULL)
	{
		return MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR os_err;
	OS_OPT opt;

	if(bAutoReload)
	{
		opt = OS_OPT_TMR_PERIODIC;
	}
	else
	{
		opt = OS_OPT_TMR_ONE_SHOT;
	}

	OSTmrCreate(
			(OS_TMR*)pTmrHandle,
			(CPU_CHAR*)pName,
			(OS_TICK)dwDelay,
			(OS_TICK)dwPeriod,
			(OS_OPT)opt,
			(OS_TMR_CALLBACK_PTR)callback,
			(void*)0,
			(OS_ERR*)&os_err);

	if(os_err != OS_ERR_NONE)
	{
		return MLS_ERROR;
	}
#endif

	return MLS_SUCCESS;
}

/*****************************************************************************************
 * mlsOsalTmrStart
 */
mlsErrorCode_t mlsOsalTmrStart(mlsTmrHandle_t *pTmrHandle, UInt32 dwTickToWait)
{
#ifdef MLS_OSAL_FREE_RTOS
	if(!xTimerStart(*pTmrHandle, dwTickToWait))
	{
		return MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR os_err;

	MLS_UNUSED_PARAMETER(dwTickToWait);

	if(DEF_TRUE != OSTmrStart(
						(OS_TMR*)pTmrHandle,
						(OS_ERR*)&os_err))
	{
		return MLS_ERROR;
	}

#endif

	return MLS_SUCCESS;
}

/*****************************************************************************************
 * mlsOsalTmrStop
 */
mlsErrorCode_t mlsOsalTmrStop(mlsTmrHandle_t *pTmrHandle, UInt32 dwTickToWait)
{
#ifdef MLS_OSAL_FREE_RTOS
	if(!xTimerStop(*pTmrHandle, dwTickToWait))
	{
		return MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR os_err;

	MLS_UNUSED_PARAMETER(dwTickToWait);

	if(DEF_TRUE != OSTmrStop(
						(OS_TMR*)pTmrHandle,
						(OS_OPT)OS_OPT_TMR_NONE,
						(void*)0,
						(OS_ERR*)&os_err))
	{
		return MLS_ERROR;
	}

#endif

	return MLS_SUCCESS;
}

/*****************************************************************************************
 * mlsOsalTmrDel
 */
mlsErrorCode_t mlsOsalTmrDel(mlsTmrHandle_t *pTmrHandle, UInt32 dwTickToWait)
{
#ifdef MLS_OSAL_FREE_RTOS
	if(!xTimerDelete(*pTmrHandle, dwTickToWait))
	{
		return MLS_ERROR;
	}
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR os_err;

	MLS_UNUSED_PARAMETER(dwTickToWait);

	if(DEF_TRUE != OSTmrDel(
						(OS_TMR*)pTmrHandle,
						(OS_ERR*)&os_err))
	{
		return MLS_ERROR;
	}
#endif

	return MLS_SUCCESS;
}

/*****************************************************************************************
 ___________________________________________Other_________________________________________
 *****************************************************************************************/
/*****************************************************************************************
 * mlsOsalDelayMs
 */
Void mlsOsalDelayMs(UInt32 dwMiliseconds)
{
	if (!gIsOsSchedulerStarted)
	{
		mlsOsalDelayMsStart(dwMiliseconds);
		return;
	}

#ifdef MLS_OSAL_FREE_RTOS
	vTaskDelay(dwMiliseconds);
#endif

#ifdef MLS_OSAL_UCOS_III
#if 0
	UInt16 hours;
	UInt16 minutes;
	UInt16 seconds;
	UInt16 ms;
	OS_ERR	err;

	hours 		= dwMiliseconds / 3600000;
	minutes 	= (dwMiliseconds % 3600000)/60000;
	seconds 	= (dwMiliseconds % 60000) / 1000;
	ms			= dwMiliseconds % 1000;
	OSTimeDlyHMSM(	(CPU_INT16U)hours,
					(CPU_INT16U)minutes,
					(CPU_INT16U)seconds,
					(CPU_INT16U)ms,
					(OS_OPT)MLSOSAL_OPT_TIME,
					(OS_ERR*)&err);
#else
	OS_ERR	err;
	OSTimeDly(
			(OS_TICK)dwMiliseconds,
			(OS_OPT)OS_OPT_TIME_DLY,
			(OS_ERR*)&err);
#endif
#endif
}

/*****************************************************************************************
 * mlsOsalDelay
 */
Void mlsOsalDelay(UInt16 wHours, UInt16 wMinutes, UInt16 wSeconds, UInt16 wMiliseconds)
{
#ifdef MLS_OSAL_FREE_RTOS
	vTaskDelay((wHours*3600000)+(wMinutes*60000)+(wSeconds*1000)+wMiliseconds);
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	OSTimeDlyHMSM(
			(CPU_INT16U)wHours,
			(CPU_INT16U)wMinutes,
			(CPU_INT16U)wSeconds,
			(CPU_INT16U)wMiliseconds,
			(OS_OPT)MLSOSAL_OPT_TIME,
			(OS_ERR*)&err);
#endif
}

/*****************************************************************************************
 * mlsOSalTimerInit
 */
Void mlsOSalTimerInit(mlsTimerHandle_t *pHandle, UInt32 dwTimerValue)
{
#ifdef MLS_OSAL_FREE_RTOS
	pHandle->startTick = xTaskGetTickCount();
	pHandle->timerValue = dwTimerValue;
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	pHandle->startTick = OSTimeGet((OS_ERR*)&err);
	pHandle->timerValue = dwTimerValue;
#endif

}

/*****************************************************************************************
 * mlsOsalTimerReset
 */
Void mlsOsalTimerReset(mlsTimerHandle_t *pHandle)
{
#ifdef MLS_OSAL_FREE_RTOS
	pHandle->startTick = xTaskGetTickCount();
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	pHandle->startTick = OSTimeGet((OS_ERR*)&err);
#endif
}

/*****************************************************************************************
 * mlsOsalTimerCheck
 */
UInt32 mlsOsalTimerCheck(mlsTimerHandle_t *pHandle)
{
	UInt32 dwCurrentTick;

#ifdef MLS_OSAL_FREE_RTOS
	dwCurrentTick = xTaskGetTickCount();
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	dwCurrentTick = OSTimeGet((OS_ERR*)&err);
#endif

	if (dwCurrentTick >= pHandle->startTick)
	{
		dwCurrentTick -= pHandle->startTick;
	}
	else
	{
		dwCurrentTick = 0xFFFFFFFF - (pHandle->startTick - dwCurrentTick) + 1;
	}
	if (dwCurrentTick >= pHandle->timerValue)
	{
		return 0;
	}
	else
	{
		return pHandle->timerValue - dwCurrentTick;
	}
}

/*****************************************************************************************
 * mlsOsalTimerElapse
 */
UInt32 mlsOsalTimerElapse(mlsTimerHandle_t *pHandle)
{
        UInt32 dwCurrentTick;

        dwCurrentTick = mlsOsalGetTickCount();
        if (dwCurrentTick >= pHandle->startTick)
        {
                dwCurrentTick -= pHandle->startTick;
        }
        else
        {
                dwCurrentTick = 0xFFFFFFFF - (pHandle->startTick - dwCurrentTick) + 1;
        }

        return dwCurrentTick;
}

/*****************************************************************************************
 * mlsOsalGetElapseTime
 */
UInt32 mlsOsalGetElapseTime(UInt32 dwFrom)
{
	UInt32 curTime = mlsOsalGetTickCount();

	if (curTime >= dwFrom)
	{
		return (curTime - dwFrom);
	}
	else
	{
		return ((UInt32)0xFFFFFFFF - dwFrom + curTime);
	}
}

/*****************************************************************************************
 * mlsOsalGetTickCount
 */
UInt32 mlsOsalGetTickCount(Void)
{
#ifdef MLS_OSAL_FREE_RTOS
	return xTaskGetTickCount();
#endif

#ifdef MLS_OSAL_UCOS_III
	OS_ERR err;
	return OSTimeGet((OS_ERR*)&err);
#endif
}

/*****************************************************************************************
 * mlsOsalCriticalSectionEnter
 */
Void mlsOsalCriticalSectionEnter(Void)
{
#ifdef MLS_OSAL_FREE_RTOS
	portENTER_CRITICAL();
#endif

#ifdef MLS_OSAL_UCOS_III
	cpuSR = CPU_SR_Save();
#endif
}

/*****************************************************************************************
 * mlsOsalCriticalSectionExit
 */
Void mlsOsalCriticalSectionExit(Void)
{
#ifdef MLS_OSAL_FREE_RTOS
	portEXIT_CRITICAL();
#endif

#ifdef MLS_OSAL_UCOS_III
	CPU_SR_Restore(cpuSR);
#endif
}

/*****************************************************************************************
 * mlsOsalInterruptDisable
 */
Void mlsOsalInterruptDisable(Void)
{
#ifdef MLS_OSAL_FREE_RTOS
	portDISABLE_INTERRUPTS();
#endif

#ifdef MLS_OSAL_UCOS_III
	CPU_IntDis();
#endif
}

/*****************************************************************************************
 * mlsOsalInterruptEnable
 */
Void mlsOsalInterruptEnable(Void)
{
#ifdef MLS_OSAL_FREE_RTOS
	portENABLE_INTERRUPTS();
#endif

#ifdef MLS_OSAL_UCOS_III
	CPU_IntEn();
#endif
}

/*******************************************************************************************
 * _____________________________________END FILE____________________________________________
 */
/**@}*/

