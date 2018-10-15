/**
 * \defgroup mlsOsal
 * @brief Template Component Module
 *
 * General Description of the template component functionality, dependencies,
 * limitations, ...
 * @{
 */

/**
 * @file mlsOsal.h
 * @brief Osal apis prototype
 *
 * This is the Osal component
 * @date
 * @author
 *
 */

#ifndef __MLSOSAL_H__
#define __MLSOSAL_H__

#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ***************************************************/
#include "mlsInclude.h"

#ifdef MLS_OSAL_FREE_RTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#endif

#ifdef MLS_OSAL_UCOS_III
#include "../uCOS-iii/uCOS-III/os.h"
#endif

/********** Constant  and compile switch definition section *******************/
#define MLS_OSAL_TEST	0

#ifdef MLS_OSAL_FREE_RTOS
#define MLSOSAL_TASK_PRIORITY_IDLE 		(tskIDLE_PRIORITY)
#define MLSOSAL_TASK_PRIORITY_MEDIUM 	(tskIDLE_PRIORITY+1)
#define MLSOSAL_TASK_PRIORITY_HIGH 	 	(tskIDLE_PRIORITY+2)
#define MLSOSAL_TASK_PRIORITY_SUPERHIGH (tskIDLE_PRIORITY+3)
#define MLSOSAL_TASK_STACKSIZE_TINI 	(tskIDLE_STACK_SIZE)
#endif

#ifdef MLS_OSAL_UCOS_III
#define MLSOSAL_TASK_PRIORITY_IDLE 		(OS_CFG_PRIO_MAX-2)
#endif

#define MLSOSAL_TASK_STACKSIZE_1KB 		(1*1024/4)
#define MLSOSAL_TASK_STACKSIZE_2KB 		(2*1024/4)
#define MLSOSAL_TASK_STACKSIZE_4KB 		(4*1024/4)
#define MLSOSAL_TASK_STACKSIZE_6KB 		(6*1024/4)
#define MLSOSAL_TASK_STACKSIZE_8KB 		(8*1024/4)
#define MLSOSAL_TASK_STACKSIZE_16KB 	(16*1024/4)
#define MLSOSAL_TASK_STACKSIZE_24KB 	(24*1024/4)
#define MLSOSAL_TASK_STACKSIZE_28KB 	(28*1024/4)
#define MLSOSAL_TASK_STACKSIZE_32KB 	(32*1024/4)

#define MLSOSAL_ROUNDROBIN_ENABLE		1u
#define MLSOSAL_ROUNDROBIN_DISABLE		0u
#define MLSOSAL_MSG_SIZE_MAX			1024

/**/
#ifdef MLS_OSAL_FREE_RTOS
#define MLSOSAL_OPT_EVENT_WAIT_ALL			(UInt16)(0x0001 << 0)
#define MLSOSAL_OPT_EVENT_WAIT_ANY			(UInt16)(0x0001 << 1)
#define MLSOSAL_OPT_EVENT_WAIT_CLR_ON_EXIT	(UInt16)(0x0001 << 2)
#define MLS_OSAL_MAX_DELAY					((mlsTickType_t) portMAX_DELAY)
#endif

#ifdef MLS_OSAL_UCOS_III
#define MLSOSAL_DIV_STACK_SIZE_LIMIT	4
#define MLSOSAL_OPT_TIME				(OS_OPT_TIME_HMSM_STRICT)
#define MLSOSAL_OPT_WAIT_BLOCKING		(OS_OPT_PEND_BLOCKING)
#define MLSOSAL_OPT_TASK_CREATE			(OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR)

#define MLSOSAL_OPT_ENQUEUE				(OS_OPT_POST_FIFO|OS_OPT_POST_NO_SCHED)
#define MLSOSAL_OPT_ENQUEUE_TO_FRONT	(OS_OPT_POST_LIFO|OS_OPT_POST_NO_SCHED)
#define MLSOSAL_OPT_DEQUEUE				(OS_OPT_PEND_BLOCKING)
#define MLSOSAL_OPT_QUEUE_DEL			(OS_OPT_DEL_ALWAYS)

#define MLSOSAL_OPT_MUTEX_LOCK			(OS_OPT_PEND_BLOCKING)
#define MLSOSAL_OPT_MUTEX_CHECK			(OS_OPT_PEND_NON_BLOCKING)
#define MLSOSAL_OPT_MUTEX_UNLOCK		(OS_OPT_POST_NO_SCHED)
#define MLSOSAL_OPT_MUTEX_DELETE		(OS_OPT_DEL_ALWAYS)

#define MLSOSAL_OPT_SEM_GET				(OS_OPT_PEND_BLOCKING)
#define MLSOSAL_OPT_SEM_SET				(OS_OPT_POST_1|OS_OPT_POST_NO_SCHED)
#define MLSOSAL_OPT_SEM_DEL				(OS_OPT_DEL_ALWAYS)

#define MLSOSAL_OPT_EVENT_DEL			(OS_OPT_DEL_ALWAYS)
#define MLSOSAL_OPT_EVENT_SET			(OS_OPT_POST_FLAG_SET|OS_OPT_POST_NO_SCHED)
#define MLSOSAL_OPT_EVENT_CLR			(OS_OPT_POST_FLAG_CLR|OS_OPT_POST_NO_SCHED)

#define MLSOSAL_OPT_EVENT_WAIT_ALL			OS_OPT_PEND_FLAG_SET_ALL
#define MLSOSAL_OPT_EVENT_WAIT_ANY			OS_OPT_PEND_FLAG_SET_ANY
#define MLSOSAL_OPT_EVENT_WAIT_CLR_ON_EXIT	OS_OPT_PEND_FLAG_CONSUME

#define MLS_OSAL_MAX_DELAY		( 0 )
#endif

#define	K_COBRA_TIMEOUT_MAX		0xfffe
#define	K_COBRA_TIMEOUT_MIN		0x1

/********** Type definition section *******************************************/
#ifdef MLS_OSAL_FREE_RTOS
typedef TaskHandle_t 		mlsTaskHandle_t;
typedef TaskFunction_t 		mlsTaskEntry_t;

/*QUEUE*/
typedef QueueHandle_t 		mlsQueueHandle_t;

/*MUTEX*/
typedef SemaphoreHandle_t 	mlsMutexHandle_t;

/*EVENT*/
typedef SemaphoreHandle_t 	mlsEventHandle_t;

/*EVENTGROUP*/
typedef EventGroupHandle_t 	mlsEventGroupHandle_t;

typedef TimerHandle_t		mlsTmrHandle_t;

typedef TimerCallbackFunction_t		mlsTmrCallbackFunc_t;

typedef UInt32 mlsEventBit_t;

typedef long mlsOsalBaseType_t;

typedef UInt32 mlsTickType_t;

/*SEMAPHORE*/
typedef SemaphoreHandle_t 	mlsSemHandle_t;

#define gTrue	( (mlsOsalBaseType_t) 1 )
#define gFalse	( (mlsOsalBaseType_t) 0 )


#endif

#ifdef MLS_OSAL_UCOS_III

typedef OS_TCB		 	mlsTaskHandle_t;

typedef OS_TASK_PTR 	mlsTaskEntry_t;

/*QUEUE*/
typedef OS_Q 			mlsQueueHandle_t;

/*MUTEX*/
typedef OS_MUTEX 		mlsMutexHandle_t;

/*EVENT*/
typedef OS_SEM 			mlsEventHandle_t;

/*EVENTGROUP*/
typedef OS_FLAG_GRP 	mlsEventGroupHandle_t;

typedef UInt32 			mlsEventBit_t;

typedef long 			mlsOsalBaseType_t;

typedef OS_TICK 		mlsTickType_t;

/*SEMAPHORE*/
typedef OS_SEM 			mlsSemHandle_t;

typedef OS_TMR			mlsTmrHandle_t;

typedef OS_TMR_CALLBACK_PTR		mlsTmrCallbackFunc_t;

#define gTrue	( (mlsOsalBaseType_t) 1 )
#define gFalse	( (mlsOsalBaseType_t) 0 )

#endif

/**
 * @brief OSAL Timer Handle Structure
 *
 * Warning: application shouldn't modify these field
 */
typedef struct
{
	UInt32	startTick; 	/**< Starting value of system when timer is started*/
	UInt32	timerValue; /**< Timeout value*/
}mlsTimerHandle_t;

/**
 * @brief OSAL Cobra Frequency Structure
 */
typedef struct
{
	unsigned int	system;	/**< System frequency */
	unsigned int	ahb;	/**< AHB frequency */
	unsigned int	apb;	/**< APB frequency */

} t_cobra_frequencies;

/********** Macro definition section*******************************************/

/********** Function declaration section **************************************/


#ifdef MLS_OSAL_UCOS_III
extern void OS_CSP_TickInit(void);
extern Void* pMsgData;
#endif

/*****************************************************************************************
 ________________________________________OSAL Initialize__________________________________
 *****************************************************************************************/
/**
 * @brief OSAL Initialize
 *
 * It must be call prior to calling other function of mlsOsal.
 *
 * @param None
 *
 * @Return
 * @retval 	MLS_SUCCESS: operation successful.
 * @retval 	MLS_ERROR: failed to initialize OSAL..
 */
mlsErrorCode_t mlsOsalInit(Void);

/*****************************************************************************************
 _________________________________________Task Schedule___________________________________
 *****************************************************************************************/
/**
 * @brief OSAL Task Scheduler Start
 *
 * OSAL Start multitasking.
 *
 * This function is typically called from startup after calling mlsOsalInit().
 *
 * @param None
 *
 * @Return
 * @retval None
 */
Void mlsOsalTaskSchedule (Void);

/**
 * @brief OSAL RoundRobin Scheduler Configuration
 *
 * Try to configuration Round_Robin mode (many task have same priority)  (Use for UCOS-III)
 *
 * @param	enable: <br>
 * 				- <b> MLSOSAL_ROUNDROBIN_ENABLE </b> Enable Round_Robin scheduling. <br>
 * 				- <b> MLSOSAL_ROUNDROBIN_DISABLE </b> Disable Round_Robin scheduling.
 * @param	timeQuanta: time switching between others task running at the same priority.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: failed to configure RoundRobin Scheduler.
 */
mlsErrorCode_t mlsOsalSchedRoundRobinCfg(UInt8 enable, mlsTickType_t timeQuanta);

/**
 * @brief OSAL RoundRobin Scheduler Yield
 *
 * Used to voluntarily give up a task's time slot.  (Use for UCOS-III)
 *
 * @param None
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: otherwise.
 */
mlsErrorCode_t mlsOsalSchedRoundRobinYield(Void);

/*****************************************************************************************
 ________________________________________Task Management__________________________________
 *****************************************************************************************/
/**
 * @brief OSAL Create Task
 *
 * Used to Creates a new task.
 *
 * @param pTaskHandle: pointer to the handle task to be created.
 * @param entry: pointer to the task - the function of the task.
 * @param pTaskName: a pointer to a String to assign a name to the task.
 * @param pStackBuf: a pointer to the stacks base address.
 * @param wStackSize: the size of stack of this task.
 * @param pParameter:  pointer to an optional data area.
 * @param wPriority: priority of the task.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: failed to create a task.
 */
mlsErrorCode_t mlsOsalTaskCreate(
							mlsTaskHandle_t*	pTaskHandle,
							mlsTaskEntry_t 		entry,
							const Char* 		pTaskName,
							UInt32*				pStackBuf,
							UInt16 				wStacksize,
							Void* 				pParameter,
							Int16 				wPriority);

/**
 * @brief OSAL Create Task with provided stack.
 *
 * Used to Creates a new task with the user provided stack.
 *
 * @param pTaskHandle: pointer to the handle task to be created.
 * @param entry: pointer to the task - the function of the task.
 * @param pTaskName: a pointer to a String to assign a name to the task.
 * @param pStackBuf: a pointer to the stacks base address.
 * @param wStackSize: the size of stack of this task.
 * @param pParameter:  pointer to an optional data area.
 * @param wPriority: priority of the task.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: failed to create a task.
 */
mlsErrorCode_t mlsOsalTaskCreateSelfManageStack(
							mlsTaskHandle_t*	pTaskHandle,
							mlsTaskEntry_t 		entry,
							const Char* 		pTaskName,
							UInt32* 			pStackBuf,
							UInt16 				wStacksize,
							Void* 				pParameter,
							Int16 				wPriority);

/**
 * @brief OSAL Task Suspend
 *
 * Try to suspend or block execution of a task.
 *
 * @param pTaskHandle: pointer to the handle task being suspends.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: failed to suspend a task.
 */
mlsErrorCode_t mlsOsalTaskSuspend(mlsTaskHandle_t* pTaskHandle);

/**
 * @brief OSAL Task Resume
 *
 * Try to resume a task suspended through the mlsOsalTaskSuspend() function.
 *
 * @param pTaskHandle: pointer to the handle task is suspended being resuming.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: failed to resume a task.
 */
mlsErrorCode_t mlsOsalTaskResume(mlsTaskHandle_t* pTaskHandle);

/**
 * @brief OSAL Task Delete
 *
 * Remove the task have been created by mlsOsaltaskCreate() function.
 *
 * @param pTaskHandle: pointer to the handle task being deleting.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: failed to delete a task.
 */
mlsErrorCode_t mlsOsalTaskDelete(mlsTaskHandle_t* pTaskHandle);

/**
 * @brief OSAL Task Change Priority
 *
 * Try to change priority of task.
 *
 * @param pTaskHandle: pointer to the handle task for which the priority is being change.
 * @param wNewPrio: the priority to which the task will be change to.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: failed to change priority of task.
 */
mlsErrorCode_t mlsOsalTaskChangePrio(mlsTaskHandle_t* pTaskHandle, Int16 wNewPrio);

/*****************************************************************************************
 ______________________________Mutual Exclusion Semaphores________________________________
 *****************************************************************************************/
/**
 * @brief OSAL Create Mutex
 *
 * Used to create and initialize a MUTEX.
 *
 * @param pMutexHandle: pointer of the MUTEX need to be created.
 * @param pName: pointer to a String used to assign a name to the MUTEX.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: failed to create a mutex.
 */
mlsErrorCode_t mlsOsalMutexCreate(mlsMutexHandle_t* pMutexHandle, const Char* pName);

/**
 * @brief OSAL Lock Mutex
 *
 * If a task calls mlsOsalMutexLock and the mutex is available, mlsOsalMutexLock give the mutex to
 * the caller and return to its caller.
 *
 * If the mutex is already owned by another task, mlsOsalMutexLock places the calling task
 * in the wait list until the task that owns the mutex releases or timeout.
 *
 * @param mutexHandle: pointer to the mutex being locked.
 * @param dwBlockTime: timeout value waiting for mutex release.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: failed to lock a mutex.
 */
mlsErrorCode_t mlsOsalMutexLock(mlsMutexHandle_t* pMutexHandle, UInt32 dwBlockTime);

/**
 * @brief OSAL Unlock Mutex
 *
 * Release the mutex if the mutex is locked by first calling mslOsalMutexLock.
 *
 * @param pMutexHandle: pointer to the mutex being unlocked.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: failed to unlock a mutex.
 */
mlsErrorCode_t mlsOsalMutexUnlock(mlsMutexHandle_t* pMutexHandle);

/**
 * @brief OSAL Check Mutex
 *
 * Require to lock the mutex.
 *
 * @param pMutexHandle: pointer to the mutex being checked.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: the mutex have been locked by another task.
 */
mlsErrorCode_t mlsOsalMutexCheck(mlsMutexHandle_t* pMutexHandle);

/**
 * @brief OSAL Delete Mutex
 *
 * Used to delete a mutex.
 *
 * @param pMutexHandle: pointer to the mutex being deleted.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: failed to delete a mutex.
 */
mlsErrorCode_t mlsOsalMutexDelete(mlsMutexHandle_t* pMutexHandle);

/*****************************************************************************************
 _________________________________________Semaphores______________________________________
 *****************************************************************************************/
/**
 * @brief OSAL Create Semaphore.
 *
 * Used to initialize (create) a semaphore.
 *
 * @param pSemHandle: pointer to the semaphore being created.
 * @param pName: pointer to a String to assign a name to the semaphore.
 * @param dwMaxCount: the maximum count value that can be reached.
 * @param dwInitialCount: the count value assigned to the semaphore when it is created.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: failed to create a semaphore.
 */
mlsErrorCode_t mlsOsalSemCreate(
							mlsSemHandle_t* pSemHandle,
							const Char* pName,
							UInt32 dwMaxCount,
							UInt32 dwInitialCount);

/**
 * @brief OSAL Semaphore Get.
 *
 * Used when a task wants exclusive access to a resource – wait until an event occurs or timeout.
 * Can not be used from an ISR
 *
 * @param pSemHandle: pointer to the semaphore being get.
 * @param dwTimeOut: timeout value.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: otherwise.
 */
mlsErrorCode_t mlsOsalSemGet(mlsSemHandle_t* pSemHandle, UInt32 dwTimeOut);

/**
 * @brief OSAL Semaphore Get from ISR.
 *
 * Used when a task wants exclusive access to a resource – wait until an event occurs or timeout.
 * Can be used from an ISR
 *
 * @param pSemHandle: pointer to the semaphore being get.
 * @param pHigherPrioTaskWoken: <br>
 * 			- <b> gTrue: </b>unblocked task has a priority higher than the currently running task. <br>
 * 			- <b> gFalse: </b> otherwise.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: otherwise.
 */
mlsErrorCode_t mlsOsalSemGetFromISR(mlsSemHandle_t* pSemHandle, mlsOsalBaseType_t *pHigherPrioTaskWoken);

/**
 * @brief OSAL Semaphore Set.
 *
 * Used to release a semaphore.
 *
 * @param pSemHandle: pointer to the semaphore being set.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: failed to set a semaphore.
 */
mlsErrorCode_t mlsOsalSemSet(mlsSemHandle_t* pSemHandle);

/**
 * @brief OSAL Semaphore Set from ISR.
 *
 * Used to release a semaphore. Can be used from an ISR.
 *
 * @param pSemHandle: pointer to the semaphore being released.
 * @param pHigherPrioTaskWoken: <br>
 * 			- <b> gTrue: </b>unblocked task has a priority higher than the currently running task. <br>
 * 			- <b> gFalse: </b> otherwise.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: failed to set a semaphore from ISR.
 */
mlsErrorCode_t mlsOsalSemSetFromISR(mlsSemHandle_t* pSemHandle, mlsOsalBaseType_t *pHigherPrioTaskWoken);

/**
 * @brief OSAL Delete Semaphore.
 *
 * Used to delete semaphore.
 *
 * @param pSemHandle: pointer to the semaphore being deleted.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: failed to delete a semaphore.
 */
mlsErrorCode_t mlsOsalSemDelete(mlsSemHandle_t* pSemHandle);

/*****************************************************************************************
 ________________________________________Event Flags______________________________________
 *****************************************************************************************/
/**
 * @brief OSAL Create Event Group.
 *
 * Used to create and initialize an event flags.
 *
 * @param pEventHandle: pointer to the event flags being created.
 * @param pName: pointer to the String to assign a name to the event flags.
 * @param dwInitialEvent: initial value of the event flags.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: failed to create an event flag.
 */
mlsErrorCode_t mlsOsalEventGroupCreate(
							mlsEventGroupHandle_t* pEventHandle,
							const Char* pName,
							UInt32 dwInitialEvent);

/**
 * @brief OSAL Event Group Set Bit.
 *
 * Used to set bit within an event flags. This function cannot be called from ISR.
 *
 * @param pEventHandle: a pointer to the event flags being set bit.
 * @param eventBits: a bitwise value that indicates the bit or bits to set in the event flags.
 *
 * @Return
 * @retval $ The value of the event flag at the time the call to mlsOsalEventGroupSetBit().
 */
mlsEventBit_t mlsOsalEventGroupSetBit(
							mlsEventGroupHandle_t* pEventHandle,
							const mlsEventBit_t eventBits);

/**
 * @brief OSAL Event Group Set Bit from ISR.
 *
 * Used to set bit within an eventFlags. This function can be called from ISR.
 *
 * @param pEventHandle: a pointer to the event flags being set bit.
 * @param eventBits: a bitwise value that indicates the bit or bits to set in the event flags.
 * @param pHigherPrioTaskWoken: <br>
 * 			- <b> gTrue: </b> if the priority of the Daemon task is higher than the priority of
 * 							  the currently running task. <br>
 * 			- <b> gFalse: </b> otherwise.
 *
 * @Return
 * @retval  MLS_SUCCESS: the message was sent to the OS daemon task.
 * @retval  MLS_ERROR: otherwise.
 */
mlsErrorCode_t mlsOsalEventGroupSetBitFromISR(
							mlsEventGroupHandle_t* pEventHandle,
							const mlsEventBit_t eventBits,
							mlsOsalBaseType_t* pHigherPrioTaskWoken);

/**
 * @brief OSAL Event Group Clear Bit.
 *
 * Used to clear bit within an event flags. This function cannot be called from ISR.
 *
 * @param pEventHandle: pointer to the event flags need to be cleared bit / bits.
 * @param eventBits: a bitwise value that indicates the bit or bits to clear in the event flags.
 *
 * @Return
 * @retval $ The value of the event flags.
 */
mlsEventBit_t mlsOsalEventGroupClearBit(
							mlsEventGroupHandle_t* pEventHandle,
							const mlsEventBit_t eventBits);

/**
 * @brief OSAL Event Group Clear Bit from ISR.
 *
 * Used to clear bit within an event flags. This function can be called from ISR.
 *
 * @param pEventHandle: pointer to the event flags need to be cleared bit / bits.
 * @param eventBits: a bitwise value that indicates the bit or bits to clear in the event flags.
 *
 * @Return
 * @retval $ The value of the event flags.
 */
mlsEventBit_t mlsOsalEventGroupClearBitFromISR(
							mlsEventGroupHandle_t* pEventHandle,
							const mlsEventBit_t eventBits);

/**
 * @brief OSAL Event Group Get Bit.
 *
 * Getting current value of the event flags. This function cannot be called from ISR.
 *
 * @param pEventHandle: pointer to the event flags need to be get value.
 *
 * @Return
 * @retval $ The value of the event flags.
 */
mlsEventBit_t mlsOsalEventGroupGetBit(mlsEventGroupHandle_t* pEventHandle);

/**
 * @brief OSAL Event Group Get Bit from ISR.
 *
 * Getting current value of the event flags. This function can be called from ISR.
 *
 * @param pEventHandle: pointer to the event flags need to be get value.
 *
 * @Return
 * @retval $ The value of the event flags.
 */
mlsEventBit_t mlsOsalEventGroupGetBitFromISR(mlsEventGroupHandle_t pEventHandle);

/**
 * @brief OSAL Event Group Wait Bit/Bits.
 *
 * Read bits within an event flags. Optionally entering the Blocked state (with a timeout)
 * to wait for a bit or group of bits to become set.
 *
 * This function can not be called from ISR.
 *
 * @param pEventHandle: a pointer to the event flags being waited.
 * @param eventBits: a bitwise value that indicates the bit / bits to wait inside the event flags.
 * @param tickToWait: the maximum amount of time (ticks) to wait for one / all (depending on the ‘option’)
 * 					  of the bits specified by event flags become set.
 * @param wOption: <br>
 * 				- <b> MLSOSAL_OPT_EVENT_WAIT_ALL:</b> check all bits in eventBit to be set. <br>
 * 				- <b> MLSOSAL_OPT_EVENT_WAIT_ANY:</b> check any bit in eventBit to be set. <br>
 * 				- <b> MLSOSAL_OPT_EVENT_WAIT_CLR_ON_EXIT: </b>
 * 						any bits set in the value passed as the ‘eventBits’ parameter
 * 						will be clear in the event flags before this function return.
 *
 * @Return
 * @retval $ The value of the event flags at the time either the event bits being waited for
 * 			  became set or the block time expired.
 */
mlsEventBit_t mlsOsalEventGroupWaitBits(
							mlsEventGroupHandle_t* pEventHandle,
							const mlsEventBit_t eventBits,
							mlsTickType_t dwTickToWait,
							UInt16 wOption);

/**
 * @brief OSAL Delete Event Group.
 *
 * Delete an event flags that was previously create using a call to mlsOsalEventGroupCreate()
 *
 * @param pEventHandle: a pointer to the event flags being deleted.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful
 * @retval  MLS_ERROR: failed to delete an event flags.
 */
mlsErrorCode_t mlsOsalEventGroupDelete(mlsEventGroupHandle_t* pEventHandle);

/*****************************************************************************************
 ________________________________________Message Queue____________________________________
 *****************************************************************************************/
/**
 * @brief OSAL Create Message Queue.
 *
 * Used to create a message queue.
 * A message queue allows task or ISRs to send pointer variables (messages) to one or more tasks.
 *
 * @param pQueueHandle: a pointer to the message queue being created.
 * @param dwQueueSize: number of item of the queue
 * @param pName: pointer to a String to assign a name to the message queue.
 * @param wItemSize: size of one item.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful
 * @retval  MLS_ERROR: failed to create a message queue.
 */
mlsErrorCode_t mlsOsalQueueCreate(
							mlsQueueHandle_t* pQueueHandle,
							UInt32 dwQueueSize,
							const Char* pName,
							UInt16 wItemSize);

/**
 * @brief OSAL Message Queue Enqueue.
 *
 * Post item to the message queue.
 *
 * @param pQueueHandle: pointer to the message queue going to take the item.
 * @param pData: pointer to item’s data.
 * @param wMsgSize: size of item’s data.
 * @param dwMaxBlockingTimeMs: maximum the blocking time of current in case the queue is full in miliseconds.
 * @param isFromISR: <br>
 * 			- <b>0x01</b>: Can be use from an ISR. <br>
 * 			- <b>0x00</b>: Can not be use from an ISR.
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: otherwise.
 */
mlsErrorCode_t mlsOsalQueueEnqueue(
							mlsQueueHandle_t* pQueueHandle,
							Void* pData,
							UInt16 wMsgSize,
							UInt32 dwMaxBlockingTimeMs,
							Bool isFromISR);

/**
 * @brief OSAL Message Queue Enqueue to front.
 *
 * Post item to the front of the message queue.
 * If the queue is not empty, the items will become the first one available when the queue is dequeued.
 *
 * @param pQueueHandle: a pointer to the message queue that going to take the item.
 * @param pData: pointer to item’s data being post to the message queue.
 * @param wMsgSize: size of item’s data being post.
 * @param dwMaxBlockingTimeMs: maximum the blocking time of current in case the queue is full in milisecond.
 * @param isFromISR: <br>
 * 			- <b>0x01</b>: Can be use from an ISR. <br>
 * 			- <b>0x00</b>: Can not be use from an ISR.
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: otherwise.
 */
mlsErrorCode_t mlsOsalQueueEnqueueToFront(
							mlsQueueHandle_t* pQueueHandle,
							Void* pData,
							UInt16 wMsgSize,
							UInt32 dwMaxBlockingTimeMs,
							Bool isFromISR);

/**
 * @brief OSAL Message Queue Dequeue.
 *
 * Getting item from the message queue.
 *
 * @param pQueueHandle: pointer to the message queue that holding data that we want to get.
 * @param pData: pointer to item’s data being get data from message queue.
 * @param wMsgSize: size of item’s data being get.
 * @param dwMaxBlockingTimeMs: maximum the blocking time of current in case the queue is empty in milisecond.
 * @param isFromISR: <br>
 * 			- <b>0x01</b>: Can be use from an ISR. <br>
 * 			- <b>0x00</b>: Can not be use from an ISR.
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: otherwise.
 */
mlsErrorCode_t mlsOsalQueueDequeue(
							mlsQueueHandle_t* pQueueHandle,
							Void* pData,
							UInt16* wMsgSize,
							UInt32 dwMaxBlockingTimeMs,
							Bool isFromISR);

/**
 * @brief OSAL Message Queue : Number of item is waiting.
 *
 * Getting current size (number of item) of the message queue.
 *
 * @param pQueueHandle: 	pointer to the message queue that we want to get the number of item is waiting.
 *
 * @Return
 * @retval $ Number of item currently presented in the message queue.
 */
UInt32 mlsOsalQueueNumberOfItemWaiting(mlsQueueHandle_t* pQueueHandle);

/**
 * @brief OSAL Delete Message Queue.
 *
 * Used to delete a message queue.
 *
 * @param pQueueHandle: pointer to the message queue being deleted.
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: otherwise.
 */
mlsErrorCode_t mlsOsalQueueDel(mlsQueueHandle_t* pQueueHandle);

/*****************************************************************************************
 ___________________________________________Timer_________________________________________
 *****************************************************************************************/
/**
 * @brief OSAL Create Timer.
 *
 * Used to Create a Software Timer.
 *
 * @param pTmrHandle: pointer to the timer handler.
 * @param pName: name of software timer.
 * @param bAutoReload: if autoReload is set to 1, then the timer will expire repeatedly
 * 					  with frequency set by the "period" parameter. If "autoReload" is
 * 					  set to 0, the timer will be a one-shot mode.
 * @param pTimeID: An identifier that is assigned to timer being create.
 * @param dwDelay: ONE_SHOT mode: timeout
 * 				 PERIODIC mode: timeout to wait before the timer configured for PERIODIC mode
 * @param dwPeriod: the period of the timer.
 * @param callback: the function to call when the timer expires
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: otherwise.
 */
mlsErrorCode_t mlsOsalTmrCreate(
							mlsTmrHandle_t *pTmrHandle,
							const char* pName,
							UInt8 bAutoReload,
							void* const pTimeID,
							UInt32 dwDelay,
							UInt32 dwPeriod,
							mlsTmrCallbackFunc_t callback);

/**
 * @brief OSAL Timer Start
 *
 * Used to start the countdown process of a timer.
 *
 * @param pTmrHandle: pointer to the timer handler.
 * @param dwTickToWait: delay time before timer start (only used for freeRTOS)
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: otherwise.
 */
mlsErrorCode_t mlsOsalTmrStart(mlsTmrHandle_t *pTmrHandle, UInt32 dwTickToWait);

/**
 * @brief OSAL Timer Stop
 *
 * Used to stop a timer.
 *
 * @param pTmrHandle: pointer to the timer handler.
 * @param dwTickToWait: delay time before timer stop (only used for freeRTOS)
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: otherwise.
 */
mlsErrorCode_t mlsOsalTmrStop(mlsTmrHandle_t *pTmrHandle, UInt32 dwTickToWait);

/**
 * @brief OSAL Timer Delete
 *
 * Used to delete a timer.
 *
 * @param pTmrHandle: pointer to the timer handler.
 * @param dwTickToWait: delay time before timer delete (only used for freeRTOS)
 *
 * @Return
 * @retval  MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: otherwise.
 */
mlsErrorCode_t mlsOsalTmrDel(mlsTmrHandle_t *pTmrHandle, UInt32 dwTickToWait);

/**
 * @brief OSAL Delay Milisecond.
 *
 * Allows a task to delay itself for a user-specified period that is specified in miliseconds.
 *
 * @param dwMiliseconds: the number of miliseconds the task is delayed.
 *
 * @Return
 * @retval None
 */
Void mlsOsalDelayMs(UInt32 dwMiliseconds);

/**
 * @brief OSAL Delay.
 *
 * Allows a task to delay itself for a user-specified period that is specified
 * in hours, minutes, seconds and miliseconds.
 *
 * @param wHours: the number of hours the task is delayed., the valid range is 0 … 99
 * @param wMinutes: the number of minutes the task is delayed, the valid range is 0 ... 59
 * @param wSeconds: the number of seconds the task is delayed, the valid range is 0 … 59.
 * @param wMiliseconds: the number of mili_second the task is delayed, the valid range is 0 … 999.
 *
 * @Return
 * @retval None
 */
Void mlsOsalDelay(UInt16 wHours, UInt16 wMinutes, UInt16 wSeconds, UInt16 wMiliseconds);

/**
 * @brief OSAL Timer Initialize.
 *
 * Used to initialize timer. Setting starting value of system when timer is started and timeout value.
 *
 * @param pTimeHandle: pointer to the OSAL timer structure.
 * @param dwTimerValue: timeout value for the OSAL timer.
 *
 * @Return
 * @retval None
 */
Void mlsOSalTimerInit(mlsTimerHandle_t *pTimeHandle, UInt32 dwTimerValue);

/**
 * @brief OSAL Timer Reset.
 *
 * Used to reset start tick to current tick.
 *
 * @param pTimeHandle: pointer to the OSAL timer structure.
 *
 * @Return
 * @retval None
 */
Void mlsOsalTimerReset(mlsTimerHandle_t *pTimeHandle);

/**
 * @brief OSAL Timer Check.
 *
 * Check if the timer is timeout
 *
 * @param pTimeHandle: pointer to the OSAL timer structure.
 *
 * @Return
 * @retval  0: Timeout
 * @retval  Otherwise: remaining millisecond for timeout.
 */
UInt32 mlsOsalTimerCheck(mlsTimerHandle_t *pTimeHandle);

/**
 * @brief OSAL Timer Get Elapse Time From.
 *
 * @param dwFrom: the value of tick time need (t) to get elapse time from t to current tick .
 *
 * @Return
 * @retval $ Elapse Time
 */
UInt32 mlsOsalGetElapseTime(UInt32 dwFrom);

/**
 * @brief OSAL Timer Get Elapse Time.
 *
 * Get elapse time from start to current tick.
 *
 * @param pTimeHandle: pointer to the OSAL timer structure.
 *
 * @Return
 * @retval $ Elapse Time
 */
UInt32 mlsOsalTimerElapse(mlsTimerHandle_t *pTimeHandle);

/**
 * @brief OSAL Timer Get Tick Count.
 *
 * Used to get current tick of the OSAL system.
 *
 * @param None
 *
 * @Return
 * @retval $ Current Tick
 */
UInt32 mlsOsalGetTickCount(Void);

/*****************************************************************************************
 __________________________________________OS System______________________________________
 *****************************************************************************************/
/**
 * @brief OSAL Critical Enter.
 *
 * Enter critical section
 *
 * @param None
 *
 * @Return
 * @retval None
 */
Void mlsOsalCriticalSectionEnter(Void);

/**
 * @brief OSAL Critical Exit.
 *
 * Exiting from a critical section
 *
 * @param None
 *
 * @Return
 * @retval None
 */
Void mlsOsalCriticalSectionExit(Void);

/**
 * @brief OSAL Disable Interrupt.
 *
 * Disable system interrupt
 *
 * @param None
 *
 * @Return
 * @retval None
 */
Void mlsOsalInterruptDisable(Void);

/**
 * @brief OSAL Enable Interrupt.
 *
 * Enable system interrupt
 *
 * @param None
 *
 * @Return
 * @retval None
 */
Void mlsOsalInterruptEnable(Void);


#ifdef __cplusplus
}
#endif

#endif /* __MLSOSAL_H__ */
/*@}*/
