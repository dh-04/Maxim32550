/*******************************************************************************
 *  (C) Copyright 2009 Molisys Solutions Co., Ltd. , All rights reserved       *
 *                                                                             *
 *  This source code and any compilation or derivative thereof is the sole     *
 *  property of Molisys Solutions Co., Ltd. and is provided pursuant to a      *
 *  Software License Agreement.  This code is the proprietary information      *
 *  of Molisys Solutions Co., Ltd and is confidential in nature.  Its use and  *
 *  dissemination by any party other than Molisys Solutions Co., Ltd is        *
 *  strictly limited by the confidential information provisions of the         *
 *  Agreement referenced above.                                                *
 ******************************************************************************/

/**
 * @defgroup mlsNetworkAbstract.c	mlsNetworkAbstract.c
 * @brief mlsNetworkAbstract.c
 *
 * Add more details about module
 * @{
 */

/**
 * @file	mlsNetworkAbstract.c
 *
 * @date 	8 Feb 2018
 * @author	stylvn008
 */


/********** Include section ***************************************************/
#include <stdio.h>
#include <string.h>
#include "../src/mlsOsal/inc/mlsOsal.h"
#include "../inc/mlsNetwork.h"
#include "../inc/mlsNetworkHw.h"
#include "../inc/mls3630.h"
#include "../inc/mlsEG95.h"
#include "../src/mlsList/inc/slist.h"
//#include "../mlsDebug/inc/mlsDebug.h"
//#include "../mlsInfo/inc/mlsInfo.h"
//#include "../mlsApp/inc/mlsRtc.h"
#include "../src/mlsList/inc/node.h"
//#include "../mlsRfApi/inc/mlsRfApiFactoryTest.h"
#include "../src/mlsLCD/inc/mlsLCDDriver.h"
#include "../src/mlsLib/inc/mlsLib.h"
/********** Local Constant and compile switch definition section **************/
#define NETWORK_TASK_PRIORITY	5
#define NETWORK_TASK_STACK_SIZE	MLSOSAL_TASK_STACKSIZE_4KB
#define X_OFFSET			5
#define Y_OFFSET			10
#define X_OFFSET_MIDDLE		55
#define LINE_OFFSET(__line)		(Y_OFFSET + 25*__line)

/********** Local Type definition section *************************************/
typedef enum
{
	MLS_NETWORK_RESET,
	MLS_NETWORK_SETUP,
	MLS_NETWORK_IDLE,
}mlsNetworkState_t;
/********** Local Macro definition section ************************************/
#define CHECK_CLOSE_BUFFER  ((gModule == ZTE_ME3630)?RESP_3630_CLOSED:RESP_EG95_CLOSED)

/********** Global variable definition section ********************************/
struct SList gNetRspList;
struct SList *gpNetRspList = Null;
UInt8 gNetworkDataRsp[AT_RESPONSE_SIZE];
mlsNetworkModuleType_t gModule = ZTE_ME3630;
mlsNetworkFunction_t gNetworkFunc;
Bool gIsCheckDown = True;
Bool gIsNetworkHang = False;
Bool gIsCheckClose = True;
Bool gIsChangedSIM = False;
UInt8 gMaxSocketId;
mlsSocketInfo_t gSocketTable[MAX_SOCKET_ID] =
{
 {
		.available = True,
		.sockID = 1,
 },
 {
		.available = True,
		.sockID = 2,
 },
 {
		.available = True,
		.sockID = 3,
 },
 {
		.available = True,
		.sockID = 4,
 },
 {
		.available = True,
		.sockID = 5,
 },
 {
        .available = True,
        .sockID = 6,
 },
 {
        .available = True,
        .sockID = 7,
 },
 {
        .available = True,
        .sockID = 8,
 },
 {
        .available = True,
        .sockID = 9,
 },
 {
        .available = True,
        .sockID = 10,
 },
 {
        .available = True,
        .sockID = 11,
 },
};

mlsMutexHandle_t gNetworkMutex;
char gAPN[MLS_INFO_APN_MAX_LEN] = "INTERNET";
/********** Local (static) variable definition section ************************/
static mlsTaskHandle_t  gNetworkHandler;
static UInt32 gNetworkSTK[NETWORK_TASK_STACK_SIZE];
static mlsNetworkState_t gNetworkState = MLS_NETWORK_RESET;
/********** Local (static) function declaration section ***********************/
/**
 * @brief This function is to thread handler of network service
 * @param[in, out] pParam is pointer to parameter input
 */
static Void mlsNetworkThreadService(Void *pParam);

/**
 * @brief This function is to check whether Network down
 * @return error code. MLS_SUCCESS if Network down
 */
static mlsErrorCode_t mlsNetworkCheckDown();

/**
 * @brief This function is to check whether Network hang or silent or fault
 * @return error code. MLS_SUCCESS if Network has problem
 */
static mlsErrorCode_t mlsNetworkCheckHang();

/**
 * @brief This function is to check whether Socket closed
 * @return error code. MLS_SUCCESS if Socket closed
 */
static mlsErrorCode_t mlsNetworkCheckClose();

static mlsErrorCode_t mlsNetworkGetSIMID(UInt8 *id, UInt8 *lenID);
/********** Local function definition section *********************************/

/********** Global function definition section ********************************/
mlsErrorCode_t mlsNetworkCheckModule(mlsNetworkModuleType_t *pModule)
{
    mlsErrorCode_t errorCode = MLS_ERR_NETWORK_FAIL;
    struct Node *node = Null;
    UInt32 tickCount;
    mlsNetworkModuleType_t _ModuleOld = *pModule;

    mlsNetworkHwWrite((UInt8 *)CHECK_MODEL_ID, strlen(CHECK_MODEL_ID));
#if DEBUG_LOG_NETWORK
    mlsDebugPrint(CHECK_MODEL_ID);
#endif
    tickCount = mlsOsalGetTickCount();
    do
    {
        node = slist_SearchNode(gpNetRspList, RESP_OK, strlen(RESP_OK));
        if (Null != node)
        {
#if DEBUG_LOG_NETWORK
            mlsDebugPrint(RESP_OK"\r\n");
#endif
            slist_RemoveNode(gpNetRspList, node);
            break;
        }
        else
        {
            mlsOsalDelayMs(5);
        }
    }while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

    tickCount = mlsOsalGetTickCount();
    do
    {
        node = slist_SearchNode(gpNetRspList, MODULE_ME3630, strlen(MODULE_ME3630));
        if (Null != node)
        {
#if DEBUG_LOG_NETWORK
            mlsDebugPrint(MODULE_ME3630"\r\n");
#endif
            slist_RemoveNode(gpNetRspList, node);
            *pModule = ZTE_ME3630;
            errorCode = MLS_SUCCESS;
            break;
        }
        else
        {
            node = slist_SearchNode(gpNetRspList, MODULE_EG95, strlen(MODULE_EG95));
            if (Null != node)
            {
#if DEBUG_LOG_NETWORK
                mlsDebugPrint(MODULE_EG95"\r\n");
#endif
                slist_RemoveNode(gpNetRspList, node);
                *pModule = EG95;
                errorCode = MLS_SUCCESS;
                break;
            }
            else
            {
                mlsOsalDelayMs(5);
            }
        }
    }while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

    if ((errorCode == MLS_SUCCESS) && (_ModuleOld != *pModule))
    {
        errorCode = mlsNetworkSetFunc(*pModule);
    }

    return errorCode;
}

mlsErrorCode_t mlsNetworkCheckReady()
{
	mlsErrorCode_t errorCode;

	errorCode = (*gNetworkFunc.pFuncCheckReady)();

	return errorCode;
}

mlsErrorCode_t mlsNetworkSetAPN(const char *pAPN)
{
	mlsErrorCode_t errorCode;

	errorCode = (*gNetworkFunc.pFuncSetAPN)(pAPN);

	return errorCode;
}

mlsErrorCode_t mlsNetworkAct()
{
	mlsErrorCode_t errorCode;

	errorCode = (*gNetworkFunc.pFuncAct)();

	return errorCode;
}

static mlsErrorCode_t mlsNetworkCheckDown()
{
	mlsErrorCode_t errorCode = MLS_ERROR;
	struct Node *node = Null;
	char buff[50] = {0};

	if (gIsCheckDown == False)
	{
		return MLS_ERROR;
	}

	if (gModule == ZTE_ME3630)
	{
	    node = slist_SearchNode(gpNetRspList, RESP_3630_NETWORK_DOWN,
	                            strlen(RESP_3630_NETWORK_DOWN));
	}
	else
	{
	    sprintf(buff, RESP_EG95_NETWORK_DOWN, CTX_ID);
        node = slist_SearchNode(gpNetRspList, buff, (UInt16)strlen(buff));
	}

	if (Null != node)
	{
#if DEBUG_LOG_NETWORK
		mlsDebugPrint("Network disconnected! Reconnecting..\r\n");
#endif
		slist_RemoveNode(gpNetRspList, node);
		errorCode = MLS_SUCCESS;
	}

	return errorCode;
}

static mlsErrorCode_t mlsNetworkCheckHang()
{
	mlsErrorCode_t errorCode = MLS_ERROR;

    if (MLS_NETWORK_IDLE != gNetworkState)
    {
        return MLS_ERROR;
    }

	if (gIsNetworkHang == True)
	{
#if DEBUG_LOG_NETWORK
		mlsDebugPrint("Network hang! Check device again\r\n");
#endif
		gIsNetworkHang = False;
		errorCode = MLS_SUCCESS;
	}

	return errorCode;
}

static mlsErrorCode_t mlsNetworkCheckClose()
{
	mlsErrorCode_t errorCode = MLS_ERROR;
    struct Node *node = Null;
	char buffer[50];
	UInt8 i;

	if (gIsCheckClose == True)
	{
		for (i = 0; i < gMaxSocketId; i++)
		{
            sprintf(buffer, CHECK_CLOSE_BUFFER, gSocketTable[i].sockID);

			node = slist_SearchNode(gpNetRspList, buffer, (UInt16)strlen(buffer));
			if (Null != node)
			{
#if DEBUG_LOG_NETWORK
				mlsDebugPrint("Socket %d closed!\r\n", gSocketTable[i].sockID);
#endif
				slist_RemoveNode(gpNetRspList, node);
				gSocketTable[i].available = True;
				errorCode = MLS_SUCCESS;
			}
		}
	}

	return errorCode;
}

static Void mlsNetworkThreadService(Void *pParam)
{
	mlsErrorCode_t errorCode;
	const UInt32 RESET_TIME_MS = 15000;
	const UInt32 SET_UP_TIME_MS = 5000;
	UInt32 networkResetTick = mlsOsalGetTickCount();
	UInt32 networkSetUpTick = networkResetTick;
	UInt8 simId[MLS_INFO_ID_USIM_MAX_LEN];

	memset(gAPN, 0, sizeof(gAPN));
	memcpy_s(gAPN, sizeof(gAPN), mlsInfoSettings.Settings.APN, strlen(mlsInfoSettings.Settings.APN));

	while (1)
	{
		while(mlsInfoSettings.Settings.IsFactoryTest)
		{
			if (gZteTestStart)
			{
				gZteTestStart = False;
				gZteErrorCode = RFAPI_ZTE_PROCESSING;
				gNetworkState = MLS_NETWORK_RESET;
				networkResetTick = mlsOsalGetTickCount();
				break;
			}

			if (gZteErrorCode == RFAPI_ZTE_PROCESSING)
			{
				break;
			}
			mlsOsalDelayMs(10);
		}

		switch (gNetworkState)
		{
		case MLS_NETWORK_RESET:
			if (mlsInfoSettings.Settings.IsFactoryTest)
			{
				mlsLCDPuts(X_OFFSET, Y_OFFSET, "HARDWARE CHECK", &Arial_16,
						LCD_BLACK, LCD_WHITE, 5);
			}
			// Check network is ready
			errorCode = mlsNetworkCheckReady();
			if (MLS_SUCCESS == errorCode)
			{
				gNetworkState = MLS_NETWORK_SETUP;
				networkSetUpTick = mlsOsalGetTickCount();
			}

			/* If the time that network not working hit RESET_TIME_MS, will do hard reset */
			if (mlsOsalGetElapseTime(networkResetTick) > RESET_TIME_MS)
			{
				// Reset hardware
				mlsNetworkHwReset();

				// Update the networkResetTick for next time checking to prevent reset after reset
				networkResetTick = mlsOsalGetTickCount();
			}
			break;
		case MLS_NETWORK_SETUP:
			if (mlsInfoSettings.Settings.IsFactoryTest)
			{
				mlsLCDPuts(X_OFFSET, LINE_OFFSET(1), "HARDWARE CHECK OK", &Arial_16,
						LCD_BLACK, LCD_WHITE, 5);
				mlsLCDPuts(X_OFFSET, LINE_OFFSET(2), "NETWORK CHECK", &Arial_16,
						LCD_BLACK, LCD_WHITE, 5);
			}

            mlsNetworkCheckModule(&gModule);

            memset(simId, 0, sizeof(simId));
            errorCode = mlsNetworkGetSIMID(simId, &(mlsInfoSettings.Settings.lenIDUSIM));
			if ((MLS_SUCCESS == errorCode) && (0 != memcmp(simId, mlsInfoSettings.Settings.idUSIM, mlsInfoSettings.Settings.lenIDUSIM)))
			{
				gIsChangedSIM = True;
				memset(mlsInfoSettings.Settings.idUSIM, 0, sizeof(mlsInfoSettings.Settings.idUSIM));
				memcpy_s(mlsInfoSettings.Settings.idUSIM, sizeof(mlsInfoSettings.Settings.idUSIM), simId, mlsInfoSettings.Settings.lenIDUSIM);
#if DEBUG_LOG_NETWORK
				mlsDebugPrint("SIM has changed\r\n");
#endif
				mlsInfoUpdate(&mlsInfoSettings);
			}

			errorCode = mlsNetworkSetAPN(gAPN);
			if (MLS_SUCCESS == errorCode)
			{
				errorCode = mlsNetworkAct();
				if (MLS_SUCCESS == errorCode)
				{
				    //Ignore check hang after setup network
				    gIsNetworkHang = False;
					gNetworkState = MLS_NETWORK_IDLE;

					if (mlsInfoSettings.Settings.IsFactoryTest)
					{
						mlsLCDPuts(X_OFFSET, LINE_OFFSET(3), "NETWORK CHECK OK", &Arial_16,
								LCD_BLACK, LCD_WHITE, 5);
						gZteErrorCode = RFAPI_COMMAND_EXECUTE_SUCCESS;
					}
				}
			}

            /* If set up time is too long, this means hardware hang up. We have to reset */
            if ((mlsOsalGetElapseTime(networkSetUpTick) > SET_UP_TIME_MS) &&
                    (MLS_SUCCESS != errorCode))//No longer check for reset if setup success
            {
                gNetworkState = MLS_NETWORK_RESET;
				// Reset hardware
				mlsNetworkHwReset();
				networkResetTick = mlsOsalGetTickCount();
            }

			break;
		case MLS_NETWORK_IDLE:
			// Dequeue and handle request if get request from application

			break;
		default:
			break;
		}

		// Detect network down. Network Service will set up network and connect again
		errorCode = mlsNetworkCheckDown();
		if (MLS_SUCCESS == errorCode)
		{
			gNetworkState = MLS_NETWORK_RESET;
			mlsNetworkHwReset();
			networkResetTick = mlsOsalGetTickCount();
		}

		// If network hardware detects hang or silent or fault, Network Service will reset it
		errorCode = mlsNetworkCheckHang();
		if (MLS_SUCCESS == errorCode)
		{
			networkResetTick = mlsOsalGetTickCount();
			gNetworkState = MLS_NETWORK_RESET;
		}

		//Detect whether socket closed
		errorCode = mlsNetworkCheckClose();
		if (MLS_SUCCESS == errorCode)
		{

		}

		mlsOsalDelayMs(30);
	}
}

static mlsErrorCode_t mlsNetworkGetSIMID(UInt8 *id, UInt8 *lenID)
{
	mlsErrorCode_t errorCode;

	errorCode = (*gNetworkFunc.pFuncGetSIMID)(id, lenID);

	return errorCode;
}

mlsErrorCode_t mlsNetworkInit()
{
	mlsErrorCode_t errorCode;

	slist_Init(&gNetRspList);

	gpNetRspList = &gNetRspList;

	mlsOsalMutexCreate(&gNetworkMutex, "gNetworkMutex");

	errorCode = mlsNetworkSetFunc(gModule);
	if (MLS_SUCCESS != errorCode)
	{
		return errorCode;
	}

	/* Create thread network as the service working underground */
	mlsOsalTaskCreateSelfManageStack(&gNetworkHandler,
									  mlsNetworkThreadService,
									  "Network service",
									  gNetworkSTK,
									  NETWORK_TASK_STACK_SIZE,
									  (Void*)0,
									  NETWORK_TASK_PRIORITY);
	return errorCode;
}

Int32 mlsNetworkCreateSocket(Char *ip, Char *port)
{
	mlsErrorCode_t errorCode;

	while (!mlsNetworkIsReady())
	{
		mlsOsalDelayMs(100);
	}

	mlsOsalMutexLock(&gNetworkMutex, MLS_OSAL_MAX_DELAY);

	errorCode = (gNetworkFunc.pFuncCreateSocket)(ip, port);

	mlsOsalMutexUnlock(&gNetworkMutex);

	return errorCode;
}

mlsErrorCode_t mlsNetworkWriteSocket(Int32 sockID, Char *buffer, UInt16 bufferLen)
{
	mlsErrorCode_t errorCode;

	while (!mlsNetworkIsReady())
	{
		mlsOsalDelayMs(100);
	}

	mlsOsalMutexLock(&gNetworkMutex, MLS_OSAL_MAX_DELAY);

	errorCode = (gNetworkFunc.pFuncWriteSocket)(sockID, buffer, bufferLen);

	mlsOsalMutexUnlock(&gNetworkMutex);

	return errorCode;
}

mlsErrorCode_t mlsNetworkReadSocket(Int32 sockID, Char *buffer, UInt16 *bufferLen)
{
	mlsErrorCode_t errorCode;

	while (!mlsNetworkIsReady())
	{
		mlsOsalDelayMs(100);
	}

	mlsOsalMutexLock(&gNetworkMutex, MLS_OSAL_MAX_DELAY);

	errorCode = (gNetworkFunc.pFuncReadSocket)(sockID, buffer, bufferLen);

	mlsOsalMutexUnlock(&gNetworkMutex);

	return errorCode;
}

mlsErrorCode_t mlsNetworkCloseSocket(Int32 sockID)
{
	mlsErrorCode_t errorCode;

	while (!mlsNetworkIsReady())
	{
		mlsOsalDelayMs(100);
	}

	mlsOsalMutexLock(&gNetworkMutex, MLS_OSAL_MAX_DELAY);

	errorCode = (gNetworkFunc.pFuncCloseSocket)(sockID);

	mlsOsalMutexUnlock(&gNetworkMutex);

	return errorCode;
}

mlsErrorCode_t mlsNetworkGetRSSI(UInt32 *rssi)
{
	mlsErrorCode_t errorCode;

	if (!mlsNetworkIsReady())
	{
		return MLS_ERR_NETWORK_FAIL;
	}

	mlsOsalMutexLock(&gNetworkMutex, MLS_OSAL_MAX_DELAY);

	errorCode = (gNetworkFunc.pFuncGetRSSI)(rssi);

	mlsOsalMutexUnlock(&gNetworkMutex);

	return errorCode;
}

Bool mlsNetworkIsReady()
{
	return (gNetworkState == MLS_NETWORK_IDLE);
}

mlsErrorCode_t mlsNetworkSetFunc(mlsNetworkModuleType_t module)
{
    mlsErrorCode_t errorCode;

    switch(module)
    {
    case ZTE_ME3630:
        gNetworkFunc.pFuncInit = &mls3630Init;
        gNetworkFunc.pFuncCheckReady = &mls3630CheckReady;
        gNetworkFunc.pFuncSetAPN = &mls3630SetAPN;
        gNetworkFunc.pFuncAct = &mls3630Act;
        gNetworkFunc.pFuncCreateSocket = &mls3630CreateSocket;
        gNetworkFunc.pFuncWriteSocket = &mls3630WriteSocket;
        gNetworkFunc.pFuncReadSocket = &mls3630ReadSocket;
        gNetworkFunc.pFuncCloseSocket = &mls3630CloseSocket;
        gNetworkFunc.pFuncGetRSSI = &mls3630GetRSSI;
        gNetworkFunc.pFuncGetSIMID = &mls3630GetSIMID;
        gMaxSocketId = MAX_3630_SOCKET;
        break;
    case EG95:
        gNetworkFunc.pFuncInit = &mlsEG95Init;
        gNetworkFunc.pFuncCheckReady = &mlsEG95CheckReady;
        gNetworkFunc.pFuncSetAPN = &mlsEG95SetAPN;
        gNetworkFunc.pFuncAct = &mlsEG95Act;
        gNetworkFunc.pFuncCreateSocket = &mlsEG95CreateSocket;
        gNetworkFunc.pFuncWriteSocket = &mlsEG95WriteSocket;
        gNetworkFunc.pFuncReadSocket = &mlsEG95ReadSocket;
        gNetworkFunc.pFuncCloseSocket = &mlsEG95CloseSocket;
        gNetworkFunc.pFuncGetRSSI = &mlsEG95GetRSSI;
        gNetworkFunc.pFuncGetSIMID = &mlsEG95GetSIMID;
        gMaxSocketId = MAX_EG95_SOCKET;
        break;
    default:
        break;
    }

    errorCode = (*gNetworkFunc.pFuncInit)();

    return errorCode;
}

mlsErrorCode_t mlsNetworkNodeRemoveAll()
{
#if USE_STATIC_BUFFER
	UInt8 idx;
	struct Node *node = Null;

	for (idx = 0; idx < MAX_STATIC_NODE; idx++)
	{
		if (gNodeStatus[idx] != 0)
		{
#if DEBUG_LOG_NETWORK
			mlsDebugPrint("Exist node %d:\r\nClear unused resp with length: %d\r\n", idx, ((struct NodeData *) gNode[idx].data)->len);
#endif
			node = slist_SearchNode(&gNetRspList, ((struct NodeData *) gNode[idx].data)->data, ((struct NodeData *) gNode[idx].data)->len);
			slist_RemoveNode(&gNetRspList, node);
		}
	}
#endif

	return MLS_SUCCESS;
}

mlsErrorCode_t mlsNetworkGetTime(UInt32 *utc)
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_FAIL;
	char buffer[50] = {0};
	struct Node *node = Null;
	UInt32 tickCount;
	mlsDateTime_t time;

	mlsNetworkHwWrite((UInt8 *)"AT+CCLK?\r\n", (UInt16)strlen("AT+CCLK?\r\n"));
#if DEBUG_LOG_NETWORK
	mlsDebugPrint("AT+CCLK?\r\n");
#endif

    tickCount = mlsOsalGetTickCount();
	do
	{
		node = slist_SearchNode(gpNetRspList, "+CCLK: \"", strlen("+CCLK: \""));
		if (Null != node)
		{
			memcpy(buffer, ((struct NodeData *)node->data)->data, ((struct NodeData *)node->data)->len);
#if DEBUG_LOG_NETWORK
			mlsDebugPrint("%s\r\n", buffer);
#endif
			slist_RemoveNode(gpNetRspList, node);
			time.year = ((buffer[8] - '0') * 10) + (buffer[9] - '0') + 2000;
			time.month = ((buffer[11] - '0') * 10) + (buffer[12] - '0');
			time.dayOfMonth = ((buffer[14] - '0') * 10) + (buffer[15] - '0');
			time.hour = ((buffer[17] - '0') * 10) + (buffer[18] - '0');
			time.minute = ((buffer[20] - '0') * 10) + (buffer[21] - '0');
			time.second = ((buffer[23] - '0') * 10) + (buffer[24] - '0');
			mlsGregorianTimeToUnixTime(utc, time);
			errorCode = MLS_SUCCESS;
			break;
		}
		else
		{
			mlsOsalDelayMs(5);
		}
	}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

    tickCount = mlsOsalGetTickCount();
	do
	{
		node = slist_SearchNode(gpNetRspList, RESP_OK, strlen(RESP_OK));
		if (Null != node)
		{
#if DEBUG_LOG_NETWORK
			mlsDebugPrint(RESP_OK"\r\n");
#endif
			slist_RemoveNode(gpNetRspList, node);
			break;
		}
		else
		{
			mlsOsalDelayMs(5);
			node = slist_SearchNode(gpNetRspList, RESP_CME_ERROR, strlen(RESP_CME_ERROR));
			if (Null != node)
			{
#if DEBUG_LOG_NETWORK
				mlsDebugPrint(RESP_CME_ERROR"\r\n");
#endif
				slist_RemoveNode(gpNetRspList, node);
				break;
			}
		}
	}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

	return errorCode;
}
/**@}*/
