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
 * @defgroup mlsEG95.c	mlsEG95.c
 * @brief mlsEG95.c
 *
 * Add more details about module
 * @{
 */

/**
 * @file	mlsEG95.c
 *
 * @date 	15 Jun 2018
 * @author	sirius
 */

/********** Include section ***************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/mlsEG95.h"
#include "../inc/mlsNetwork.h"
#include "../inc/mlsNetworkHw.h"
#include "../mlsOsal/inc/mlsOsal.h"
#include "../mlsList/inc/node.h"
#include "../mlsList/inc/slist.h"
#include "../mlsDebug/inc/mlsDebug.h"
/********** Local Constant and compile switch definition section **************/

/********** Local Type definition section *************************************/
typedef enum
{
	PROCESS_START_CR,
	PROCESS_START_LF,
	PROCESS_DATA,
	PROCESS_START_SEND,
	PROCESS_START_RECV,
	PROCESS_END_LF,
}mlsEG95ProcessRsp_t;

/********** Local Macro definition section ************************************/
#if DEBUG_LOG_NETWORK
#define EG95_DEBUG_PRINTF(...)     mlsDebugPrint(__VA_ARGS__)
#else
#define EG95_DEBUG_PRINTF(...)
#endif
/********** Global variable definition section ********************************/

/********** Local (static) variable definition section ************************/
static mlsEG95ProcessRsp_t gEG95ProcessRspState = PROCESS_START_CR;
static UInt16 gEG95DataRspLen = 0;
static UInt8 _cntComma;
static UInt32 dataLenRecv = 0;
static UInt32 gEG95DataRspTickCount = 0;

/********** Local (static) function declaration section ***********************/
static inline Void mlsEG95ProcessRsp(UInt8 data);
/********** Local function definition section *********************************/
static inline Void mlsEG95ProcessRsp(UInt8 data)
{
	switch (gEG95ProcessRspState)
	{
	case PROCESS_START_CR:
		if (AT_DATA_CR == data)
		{
			gEG95ProcessRspState = PROCESS_START_LF;
		}
		break;
	case PROCESS_START_LF:
		if (AT_DATA_LF == data)
		{
			gEG95ProcessRspState = PROCESS_DATA;
			gEG95DataRspLen = 0;
		}
		break;
	case PROCESS_DATA:
		if (AT_DATA_CR != data)
		{
			gNetworkDataRsp[gEG95DataRspLen] = data;
			gEG95DataRspLen ++;
			if (AT_DATA_SEND == data)
			{
				gEG95ProcessRspState = PROCESS_START_SEND;
			}
			if ((0 == memcmp(gNetworkDataRsp, RESP_EG95_RECV, strlen(RESP_EG95_RECV)))
			        && (gEG95DataRspLen == strlen(RESP_EG95_RECV)))
			{
			    _cntComma = 0;
				dataLenRecv = 0;
				gEG95DataRspTickCount = mlsOsalGetTickCount();
				gEG95ProcessRspState = PROCESS_START_RECV;
			}
		}
		else
		{
			gEG95ProcessRspState = PROCESS_END_LF;
		}
		break;
	case PROCESS_END_LF:
		if (AT_DATA_LF == data)
		{
			if (gEG95DataRspLen != 0)
			{
				// Create a node
				struct Node *node = NodeCreate(gNetworkDataRsp, gEG95DataRspLen);

				// Insert node to list
				if (Null != node)
				{
					slist_InsertLast(gpNetRspList, node);
				}

				gEG95ProcessRspState = PROCESS_START_CR;
			}
			else
			{
				gEG95ProcessRspState = PROCESS_DATA;
			}
		}
		break;
	case PROCESS_START_SEND:
		if (' ' == data)
		{
			// Create a node
			struct Node *node = NodeCreate("\r\n> ", 4);

			// Insert node to list
			if (Null != node)
			{
				slist_InsertLast(gpNetRspList, node);
			}
		}
		gEG95ProcessRspState = PROCESS_START_CR;
		break;
	case PROCESS_START_RECV:
		gNetworkDataRsp[gEG95DataRspLen] = data;
		gEG95DataRspLen ++;
		if (((',' == data) || (AT_DATA_CR == data) || (AT_DATA_LF == data))
		        && (_cntComma < 4))
		{
		    _cntComma++;
			break;
		}
		switch (_cntComma)
		{
		    case RECV_ID:
		        break;
		    case RECV_LEN:
		        dataLenRecv = (dataLenRecv * 10) + data - '0';
		        break;
		    case RECV_DATA:
	            dataLenRecv--;
	            if (dataLenRecv == 0)
	            {
	                gEG95ProcessRspState = PROCESS_END_LF;
	            }
		        break;
		    default:
		        break;
		}
		if (mlsOsalGetElapseTime(gEG95DataRspTickCount) > SOCKET_EG95_TIMEOUT_MS)
		{
			gEG95ProcessRspState = PROCESS_START_LF;
		}
		break;
	default:
		break;
	}
}
/********** Global function definition section ********************************/

mlsErrorCode_t mlsEG95Init()
{
	mlsErrorCode_t errorCode = MLS_SUCCESS;

	/* Initialize network hardware */
	errorCode = mlsNetworkHwInit(mlsEG95ProcessRsp);

	if (MLS_SUCCESS != errorCode)
	{
		return errorCode;
	}

	return errorCode;
}

mlsErrorCode_t mlsEG95CheckReady()
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_FAIL;
	struct Node *node = Null;
	UInt32 tickCount;

    tickCount = mlsOsalGetTickCount();
    do
    {
        node = slist_SearchNode(gpNetRspList, RESP_EG95_READY, strlen(RESP_EG95_READY));
        if (Null != node)
        {
            EG95_DEBUG_PRINTF(RESP_EG95_READY"\r\n");
            slist_RemoveNode(gpNetRspList, node);
            break;
        }
        else
        {
            mlsOsalDelayMs(5);
        }
    }while(mlsOsalGetElapseTime(tickCount) < CHECK_EG95_READY_TIMEOUT_MS);

	mlsNetworkHwWrite((UInt8 *)CHECK_EG95_READY_CR_LF, strlen(CHECK_EG95_READY_CR_LF));
	EG95_DEBUG_PRINTF(CHECK_EG95_READY_CR_LF);

    tickCount = mlsOsalGetTickCount();
	do
	{
		node = slist_SearchNode(gpNetRspList, RESP_OK, strlen(RESP_OK));
		if (Null != node)
		{
			EG95_DEBUG_PRINTF(RESP_OK"\r\n");
			slist_RemoveNode(gpNetRspList, node);
			errorCode = MLS_SUCCESS;
			break;
		}
		else
		{
			mlsOsalDelayMs(5);
		}
	}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

	if (errorCode == MLS_SUCCESS)
	{
		errorCode = MLS_ERR_NETWORK_FAIL;

		mlsNetworkHwWrite((UInt8 *)DISABLE_EG95_COMMAND_ECHO, strlen(DISABLE_EG95_COMMAND_ECHO));
		EG95_DEBUG_PRINTF(DISABLE_EG95_COMMAND_ECHO);

        tickCount = mlsOsalGetTickCount();
		do
		{
			node = slist_SearchNode(gpNetRspList, RESP_OK, strlen(RESP_OK));
			if (Null != node)
			{

				EG95_DEBUG_PRINTF(RESP_OK"\r\n");

				slist_RemoveNode(gpNetRspList, node);
				errorCode = MLS_SUCCESS;
				break;
			}
			else
			{
				mlsOsalDelayMs(5);
			}
		}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);
	}

	mlsNetworkNodeRemoveAll();

	return errorCode;
}

mlsErrorCode_t mlsEG95SetAPN(const char *pAPN)
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_FAIL;
	char buffer[50];
	struct Node *node = Null;
	UInt32 tickCount;

	sprintf(buffer, SET_APN_EG95, CTX_ID, CTX_TYPE, pAPN);
	EG95_DEBUG_PRINTF(buffer);
	mlsNetworkHwWrite((UInt8 *)buffer, (UInt16)strlen(buffer));

    tickCount = mlsOsalGetTickCount();
	do
	{
		node = slist_SearchNode(gpNetRspList, RESP_OK, strlen(RESP_OK));
		if (Null != node)
		{

			EG95_DEBUG_PRINTF(RESP_OK"\r\n");

			slist_RemoveNode(gpNetRspList, node);
			errorCode = MLS_SUCCESS;
			break;
		}
		else
		{
			mlsOsalDelayMs(5);
		}
	}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

	return errorCode;
}

mlsErrorCode_t mlsEG95Act()
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_NONE_DATA;
	struct Node *node = Null;
    char buffer[50] = {0};
	UInt32 tickCount;

	mlsNetworkHwWrite((UInt8 *)QUERY_EG95_NETWORK_STATE, strlen(QUERY_EG95_NETWORK_STATE));
	EG95_DEBUG_PRINTF(QUERY_EG95_NETWORK_STATE);

	tickCount = mlsOsalGetTickCount();
	do
	{
		node = slist_SearchNode(gpNetRspList, RESP_OK, strlen(RESP_OK));
		if (Null != node)
		{
			EG95_DEBUG_PRINTF(RESP_OK"\r\n");
			slist_RemoveNode(gpNetRspList, node);
			break;
		}
		else
		{
			mlsOsalDelayMs(5);
		}
	}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

	sprintf(buffer, RESP_EG95_ACT, CTX_ID, ACTIVATED, CTX_TYPE);
	tickCount = mlsOsalGetTickCount();
	do
	{
		node = slist_SearchNode(gpNetRspList, buffer, (UInt16)strlen(buffer));
		if (Null != node)
		{
			slist_RemoveNode(gpNetRspList, node);
            EG95_DEBUG_PRINTF("PDP context state is Activated\r\n");
            return MLS_SUCCESS;
		}
        else
        {
            mlsOsalDelayMs(5);
        }
	}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

    EG95_DEBUG_PRINTF("PDP context state is not activated\r\n");

	gIsCheckDown = False;

    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, ACTIVE_EG95_NETWORK, CTX_ID);
    mlsNetworkHwWrite((UInt8 *)buffer, (UInt16)strlen(buffer));
    EG95_DEBUG_PRINTF("%s\r\n", buffer);

    tickCount = mlsOsalGetTickCount();
    do
    {
        node = slist_SearchNode(gpNetRspList, RESP_OK, strlen(RESP_OK));
        if (Null != node)
        {
            EG95_DEBUG_PRINTF(RESP_OK"\r\n");
            slist_RemoveNode(gpNetRspList, node);
            errorCode = MLS_SUCCESS;
            break;
        }
        else
        {
            node = slist_SearchNode(gpNetRspList, RESP_ERROR, strlen(RESP_ERROR));
            if (Null != node)
            {
                EG95_DEBUG_PRINTF(RESP_ERROR"\r\n");
                slist_RemoveNode(gpNetRspList, node);
                errorCode = MLS_ERR_NETWORK_FAIL;
                break;
            }
            mlsOsalDelayMs(5);
        }
    }while(mlsOsalGetElapseTime(tickCount) < ACTIVE_EG95_TIMEOUT_MS);

    gIsCheckDown = True;

	return errorCode;
}

Int32 mlsEG95CreateSocket(Char *ip, Char *port)
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_NONE_DATA;
	char buffer[100];
	struct Node *node = Null;
	int sockID = 0;
	UInt32 tickCount;
	Int8 i;
	UInt8 SocketStateIdx;
	UInt8 cntComma;
	UInt8 retryCount = 3;

	for (i = 0; i < MAX_EG95_SOCKET; i++)
	{
		if (gSocketTable[i].available == True)
		{
			sockID = gSocketTable[i].sockID;
			errorCode = MLS_SUCCESS;
			break;
		}
	}

	gIsCheckClose = False;

	if (errorCode != MLS_SUCCESS)
	{
		EG95_DEBUG_PRINTF("All socket are unavailable. Check socket again!\r\n");

		for (i = MAX_EG95_SOCKET - 1; i >= 0; i--)
		{
		    errorCode = MLS_ERR_NETWORK_NONE_DATA;
            gSocketTable[i].available = True;

		    memset(buffer, 0, sizeof(buffer));
			sprintf(buffer, QUERY_SOCKET_STATE, QUERY_TYPE, gSocketTable[i].sockID);
			mlsNetworkHwWrite((UInt8 *)buffer, (UInt16)strlen(buffer));
			EG95_DEBUG_PRINTF(buffer);

			tickCount = mlsOsalGetTickCount();
			do
			{
				node = slist_SearchNode(gpNetRspList, RESP_OK, strlen(RESP_OK));
				if (Null != node)
				{
					EG95_DEBUG_PRINTF(RESP_OK"\r\n");
					slist_RemoveNode(gpNetRspList, node);
					break;
				}
				else
				{
					mlsOsalDelayMs(5);
				}
			}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

			memset(buffer, 0, sizeof(buffer));
			sprintf(buffer, RESP_EG95_QUERY_STATE, gSocketTable[i].sockID);
			node = slist_SearchNode(gpNetRspList, buffer, (UInt16)strlen(buffer));
			if (Null != node)
			{
				memset(buffer, 0, sizeof(buffer));
				memcpy(buffer, ((struct NodeData *)node->data)->data,
						((struct NodeData *)node->data)->len);
				EG95_DEBUG_PRINTF("%s\r\n", buffer);
				slist_RemoveNode(gpNetRspList, node);

				cntComma = 0;

				//Find the socket state position
				for (SocketStateIdx = 0; SocketStateIdx < strlen(buffer); SocketStateIdx++)
				{
					if (',' == buffer[SocketStateIdx])
					{
						cntComma++;
						if (cntComma == 5)
						{
							break;
						}
					}
				}
				if (0 != memcmp((UInt8 *)"4", &buffer[SocketStateIdx + 1], 1))
				{
					EG95_DEBUG_PRINTF("Socket %d is being used\r\n", gSocketTable[i].sockID);
					gSocketTable[i].available = False;
					errorCode = MLS_ERR_NETWORK_FAIL;
				}
				else
				{
					errorCode = mlsEG95CloseSocket(gSocketTable[i].sockID);
				}
			}

			if (errorCode != MLS_ERR_NETWORK_FAIL)
			{
                sockID = gSocketTable[i].sockID;
                EG95_DEBUG_PRINTF("Socket %d now available\r\n", sockID);
			}
		}
	}

	if (sockID < 1)
	{
		EG95_DEBUG_PRINTF("[FAILED] No available socket\r\n");
		gIsCheckClose = True;
		return MLS_ERR_NETWORK_FAIL;
	}

	do
	{
	    errorCode = MLS_ERR_NETWORK_NONE_DATA;
	    sprintf(buffer, OPEN_SOCKET_EG95,
	            CTX_ID, sockID, SOCKET_EG95_MODE, ip, port, ACCESS_MODE);
	    mlsNetworkHwWrite((UInt8 *)buffer, (UInt16)strlen(buffer));
	    EG95_DEBUG_PRINTF(buffer);

	    if ((ACCESS_MODE == DIRECT_PUSH) || (ACCESS_MODE == BUFFER_ACCESS))
	    {
	        tickCount = mlsOsalGetTickCount();
	        do
	        {
	            node = slist_SearchNode(gpNetRspList, RESP_OK, strlen(RESP_OK));
	            if (Null != node)
	            {
	                EG95_DEBUG_PRINTF(RESP_OK"\r\n");
	                slist_RemoveNode(gpNetRspList, node);
	                break;
	            }
	            else
	            {
	                mlsOsalDelayMs(5);
	            }
	        }while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

	        memset(buffer, 0, sizeof(buffer));
	        sprintf(buffer, RESP_EG95_OPEN_SOCKET, sockID);
	        tickCount = mlsOsalGetTickCount();
	        do
	        {
	            node = slist_SearchNode(gpNetRspList, buffer, (UInt16)strlen(buffer));
	            if (Null != node)
	            {
	                memset(buffer, 0, sizeof(buffer));
	                memcpy(buffer, ((struct NodeData *)node->data)->data,
	                        ((struct NodeData *)node->data)->len);
	                EG95_DEBUG_PRINTF("%s\r\n", buffer);
	                slist_RemoveNode(gpNetRspList, node);

                    gSocketTable[sockID-1].available = False;
	                if (0 == memcmp((UInt8 *)"0", &buffer[strlen((char *)buffer)-1], 1))
	                {
	                    errorCode = MLS_SUCCESS;
	                }
	                else
	                {
	                    errorCode = MLS_ERR_NETWORK_FAIL;
	                    EG95_DEBUG_PRINTF("Open socket receive error\r\n");
	                }
	                break;
	            }
	            else
	            {
	                mlsOsalDelayMs(5);
	            }
	        }while(mlsOsalGetElapseTime(tickCount) < SOCKET_EG95_OPEN_TIMEOUT_MS);

            //If open socket failed, close socket and try again
	        if (errorCode == MLS_ERR_NETWORK_FAIL)
	        {
	            memset(buffer, 0, sizeof(buffer));
	            sprintf(buffer, CLOSE_SOCKET_EG95, sockID, SOCKET_CLOSE_TIMEOUT_SEC);
	            mlsNetworkHwWrite((UInt8 *)buffer, (UInt16)strlen(buffer));
	            EG95_DEBUG_PRINTF(buffer);

	            tickCount = mlsOsalGetTickCount();
	            do
	            {
	                node = slist_SearchNode(gpNetRspList, RESP_OK, strlen(RESP_OK));
	                if (Null != node)
	                {
	                    EG95_DEBUG_PRINTF(RESP_OK"\r\n");
	                    slist_RemoveNode(gpNetRspList, node);
	                    break;
	                }
	                else
	                {
	                    node = slist_SearchNode(gpNetRspList, RESP_ERROR, strlen(RESP_ERROR));
	                    if (Null != node)
	                    {
	                        EG95_DEBUG_PRINTF(RESP_ERROR"\r\n");
	                        slist_RemoveNode(gpNetRspList, node);
	                        break;
	                    }
	                    mlsOsalDelayMs(5);
	                }
	            }while(mlsOsalGetElapseTime(tickCount) < SOCKET_EG95_CLOSE_TIMEOUT_MS);
	        }

	        if (errorCode != MLS_SUCCESS)
	        {
                retryCount--;
	            EG95_DEBUG_PRINTF("Try again. Remaining time: %d\r\n", retryCount);
	        }
	        else
	        {
	            break;
	        }
	    }
	}while(retryCount);

	gIsCheckClose = True;

	if (errorCode == MLS_SUCCESS)
	{
		return sockID;
	}
	else if (errorCode == MLS_ERR_NETWORK_NONE_DATA)
	{
	    EG95_DEBUG_PRINTF("[FAILED] Hang on create socket\r\n");
		gIsNetworkHang = True;
	}

	return errorCode;
}

mlsErrorCode_t mlsEG95WriteSocket(Int32 sockID, Char *buffer, UInt16 bufferLen)
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_NONE_DATA;
	char buffer_temp[50];
	struct Node *node = Null;
	UInt32 tickCount;

	if (gSocketTable[sockID-1].available == True)
	{
	    EG95_DEBUG_PRINTF("[FAILED] Socket closed before write\r\n");
		return MLS_ERR_NETWORK_FAIL;
	}

	sprintf(buffer_temp, SOCKET_SEND_DATA, (int)sockID, bufferLen);
	mlsNetworkHwWrite((UInt8 *)buffer_temp, (UInt16)strlen(buffer_temp));
	EG95_DEBUG_PRINTF(buffer_temp);

	tickCount = mlsOsalGetTickCount();
	do
	{
		node = slist_SearchNode(gpNetRspList, "\r\n> ", 4);
		if (Null != node)
		{
			slist_RemoveNode(gpNetRspList, node);
			EG95_DEBUG_PRINTF("\r\n> ");
			errorCode = MLS_SUCCESS;
			break;
		}
		else
		{
			node = slist_SearchNode(gpNetRspList, RESP_ERROR, strlen(RESP_ERROR));
			if (Null != node)
			{
				EG95_DEBUG_PRINTF(RESP_ERROR"\r\n");
				slist_RemoveNode(gpNetRspList, node);
				errorCode = MLS_ERR_NETWORK_FAIL;
				break;
			}
			mlsOsalDelayMs(5);
		}
	}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

	if (MLS_SUCCESS == errorCode)
	{
		errorCode = MLS_ERR_NETWORK_NONE_DATA;
		mlsNetworkHwWrite((UInt8 *)buffer, bufferLen);

		tickCount = mlsOsalGetTickCount();
		do
		{
			node = slist_SearchNode(gpNetRspList, RESP_EG95_SEND_OK,
			                        strlen(RESP_EG95_SEND_OK));
			if (Null != node)
			{
				slist_RemoveNode(gpNetRspList, node);
				EG95_DEBUG_PRINTF(RESP_EG95_SEND_OK"\r\n");
				errorCode = MLS_SUCCESS;
				break;
			}
			else
			{
	            node = slist_SearchNode(gpNetRspList, RESP_EG95_SEND_FAIL,
	                                    strlen(RESP_EG95_SEND_FAIL));
	            if (Null != node)
	            {
	                slist_RemoveNode(gpNetRspList, node);
	                EG95_DEBUG_PRINTF(RESP_EG95_SEND_FAIL"\r\n");
	                errorCode = MLS_ERR_NETWORK_FAIL;
	                break;
	            }
				mlsOsalDelayMs(5);
			}
		}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);
	}

	if ((errorCode == MLS_ERR_NETWORK_NONE_DATA) && (gSocketTable[sockID-1].available == False))
	{
	    EG95_DEBUG_PRINTF("[FAILED] Hang on write socket\r\n");
		gIsNetworkHang = True;
	}

	return errorCode;
}

mlsErrorCode_t mlsEG95ReadSocket(Int32 sockID, Char *buffer, UInt16 *bufferLen)
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_NONE_DATA;
	struct Node *node = Null;
	char buffer_temp[AT_RESPONSE_SIZE] = {0};
	UInt32 tickCount;
	UInt8 cntComma = 0;
	UInt16 length = 0;
	UInt16 idx = 0;
	int i;

	sprintf(buffer_temp, RESP_EG95_RECV_SOCKID, (int)sockID);
	tickCount = mlsOsalGetTickCount();
	do
	{
		node = slist_SearchNode(gpNetRspList, buffer_temp, (UInt16)strlen(buffer_temp));
		if (Null != node)
		{
			memset(buffer_temp, 0, sizeof(buffer_temp));
			memcpy(buffer_temp, ((struct NodeData *)node->data)->data,
					((struct NodeData *)node->data)->len);
			for (i = 0; i < ((struct NodeData *)node->data)->len; i++)
			{
		        if (((',' == buffer_temp[i]) || (AT_DATA_CR == buffer_temp[i])
		                || (AT_DATA_LF == buffer_temp[i]))
		                && (cntComma < 4))
		        {
		            cntComma++;
		            continue;
		        }
		        switch (cntComma)
		        {
		            case RECV_ID:
		                break;
		            case RECV_LEN:
		                length = (UInt16)((length * 10) + buffer_temp[i] - '0');
		                break;
		            case RECV_DATA:
		                buffer[idx++] = buffer_temp[i];
		                break;
		            default:
		                break;
		        }
			}
			*bufferLen = length;
			slist_RemoveNode(gpNetRspList, node);

			memset(buffer_temp, 0, sizeof(buffer_temp));
			sprintf(buffer_temp, "OK Received! Length: %d\r\n", length);
			EG95_DEBUG_PRINTF(buffer_temp);
            errorCode = MLS_SUCCESS;
            break;
		}
		else
		{
			mlsOsalDelayMs(5);
		}

		if (gSocketTable[sockID-1].available == True)
		{
			errorCode = MLS_ERR_NETWORK_FAIL;
			EG95_DEBUG_PRINTF("[FAILED] Socket closed while reading\r\n");
			break;
		}
	}while(mlsOsalGetElapseTime(tickCount) < SOCKET_EG95_TIMEOUT_MS);

	if (errorCode == MLS_ERR_NETWORK_NONE_DATA)
	{
	    EG95_DEBUG_PRINTF("[FAILED] Hang on read socket\r\n");
		gIsNetworkHang = True;
	}

	return errorCode;
}

mlsErrorCode_t mlsEG95CloseSocket(Int32 sockID)
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_NONE_DATA;
	char buffer[50];
	struct Node *node = Null;
	UInt32 tickCount;

	gIsCheckClose = False;

	sprintf(buffer, CLOSE_SOCKET_EG95, (int)sockID, SOCKET_CLOSE_TIMEOUT_SEC);
	mlsNetworkHwWrite((UInt8 *)buffer, (UInt16)strlen(buffer));
	EG95_DEBUG_PRINTF(buffer);

	tickCount = mlsOsalGetTickCount();
	do
	{
		node = slist_SearchNode(gpNetRspList, RESP_OK, strlen(RESP_OK));
		if (Null != node)
		{
			EG95_DEBUG_PRINTF(RESP_OK"\r\n");
			slist_RemoveNode(gpNetRspList, node);
			gSocketTable[sockID-1].available = True;
			errorCode = MLS_SUCCESS;
			break;
		}
		else
		{
			node = slist_SearchNode(gpNetRspList, RESP_ERROR, strlen(RESP_ERROR));
			if (Null != node)
			{
				EG95_DEBUG_PRINTF(RESP_ERROR"\r\n");
				slist_RemoveNode(gpNetRspList, node);
				errorCode = MLS_ERR_NETWORK_FAIL;
				break;
			}
			mlsOsalDelayMs(5);
		}
	}while(mlsOsalGetElapseTime(tickCount) < SOCKET_EG95_CLOSE_TIMEOUT_MS);

	mlsNetworkNodeRemoveAll();

	if (errorCode == MLS_ERR_NETWORK_NONE_DATA)
	{
	    EG95_DEBUG_PRINTF("[FAILED] Close socket timeout! Switch to next socket ID\r\n");
	}

    gIsCheckClose = True;

	return errorCode;
}

mlsErrorCode_t mlsEG95GetRSSI(UInt32 *rssi)
{
    mlsErrorCode_t errorCode = MLS_ERR_NETWORK_NONE_DATA;
    char buffer[50];
    char rssi_str[2];
    struct Node *node = Null;
    UInt32 tickCount;
    *rssi = RSSI_VALUE_NO_NETWORK;

    do
    {
        mlsNetworkHwWrite((UInt8 *)CHECK_EG95_SIM_STATUS, strlen(CHECK_EG95_SIM_STATUS));
        EG95_DEBUG_PRINTF(CHECK_EG95_SIM_STATUS);

        tickCount = mlsOsalGetTickCount();
        do
        {
            node = slist_SearchNode(gpNetRspList, RESP_OK, strlen(RESP_OK));
            if (Null != node)
            {
                EG95_DEBUG_PRINTF(RESP_OK"\r\n");
                slist_RemoveNode(gpNetRspList, node);
                errorCode = MLS_SUCCESS;
                break;
            }
            else
            {
                mlsOsalDelayMs(5);
            }
        }while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

        if (errorCode != MLS_SUCCESS)
        {
            break;
        }

        errorCode = MLS_ERR_NETWORK_NONE_DATA;
        tickCount = mlsOsalGetTickCount();
        do
        {
            node = slist_SearchNode(gpNetRspList, RESP_EG95_CPIN_READY, strlen(RESP_EG95_CPIN_READY));
            if (Null != node)
            {
                EG95_DEBUG_PRINTF(RESP_EG95_CPIN_READY"\r\n");
                slist_RemoveNode(gpNetRspList, node);
                errorCode = MLS_SUCCESS;
                break;
            }
            else
            {
                mlsOsalDelayMs(5);
            }
        }while(mlsOsalGetElapseTime(tickCount) < CPIN_READY_TIMEOUT_MS);

        if (errorCode != MLS_SUCCESS)
        {
            break;
        }

        errorCode = MLS_ERR_NETWORK_NONE_DATA;
        mlsNetworkHwWrite((UInt8 *)EG95_CREG_REGISTRATION, strlen(EG95_CREG_REGISTRATION));
        EG95_DEBUG_PRINTF(EG95_CREG_REGISTRATION);

        tickCount = mlsOsalGetTickCount();
        do
        {
            node = slist_SearchNode(gpNetRspList, RESP_OK, strlen(RESP_OK));
            if (Null != node)
            {
                EG95_DEBUG_PRINTF(RESP_OK"\r\n");
                slist_RemoveNode(gpNetRspList, node);
                errorCode = MLS_SUCCESS;
                break;
            }
            else
            {
                mlsOsalDelayMs(5);
            }
        }while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

        if (errorCode != MLS_SUCCESS)
        {
            break;
        }

        errorCode = MLS_ERR_NETWORK_NONE_DATA;
        tickCount = mlsOsalGetTickCount();
        do
        {
            node = slist_SearchNode(gpNetRspList, RESP_CREG_REGISTERED, strlen(RESP_CREG_REGISTERED));
            if (Null != node)
            {
                memset(buffer, 0, sizeof(buffer));
                memcpy(buffer, ((struct NodeData *)node->data)->data,
                        ((struct NodeData *)node->data)->len);
                EG95_DEBUG_PRINTF("%s\r\n", buffer);
                slist_RemoveNode(gpNetRspList, node);
                if (0 == memcmp((UInt8 *)"1", &buffer[strlen((char *)buffer)-1], 1))
                {
                    errorCode = MLS_SUCCESS;
                }
                else
                {
                    errorCode = MLS_ERR_NETWORK_FAIL;
                }
                break;
            }
            else
            {
                mlsOsalDelayMs(5);
            }
        }while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

        if (errorCode != MLS_SUCCESS)
        {
            break;
        }

        errorCode = MLS_ERR_NETWORK_NONE_DATA;
        mlsNetworkHwWrite((UInt8 *)EG95_CEREG_REGISTRATION, strlen(EG95_CEREG_REGISTRATION));
        EG95_DEBUG_PRINTF(EG95_CEREG_REGISTRATION);

        tickCount = mlsOsalGetTickCount();
        do
        {
            node = slist_SearchNode(gpNetRspList, RESP_OK, strlen(RESP_OK));
            if (Null != node)
            {
                EG95_DEBUG_PRINTF(RESP_OK"\r\n");
                slist_RemoveNode(gpNetRspList, node);
                errorCode = MLS_SUCCESS;
                break;
            }
            else
            {
                mlsOsalDelayMs(5);
            }
        }while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

        if (errorCode != MLS_SUCCESS)
        {
            break;
        }

        errorCode = MLS_ERR_NETWORK_NONE_DATA;
        tickCount = mlsOsalGetTickCount();
        do
        {
            node = slist_SearchNode(gpNetRspList, RESP_CEREG, strlen(RESP_CEREG));
            if (Null != node)
            {
                memset(buffer, 0, sizeof(buffer));
                memcpy(buffer, ((struct NodeData *)node->data)->data,
                        ((struct NodeData *)node->data)->len);
                EG95_DEBUG_PRINTF("%s\r\n", buffer);
                slist_RemoveNode(gpNetRspList, node);
                if ((0 == memcmp((UInt8 *)"1", &buffer[strlen((char *)buffer)-1], 1)) ||
                        (0 == memcmp((UInt8 *)"4", &buffer[strlen((char *)buffer)-1], 1)))
                {
                    errorCode = MLS_SUCCESS;
                }
                else
                {
                    errorCode = MLS_ERR_NETWORK_FAIL;
                }
                break;
            }
            else
            {
                mlsOsalDelayMs(5);
            }
        }while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

        if (errorCode != MLS_SUCCESS)
        {
            break;
        }

        errorCode = MLS_ERR_NETWORK_NONE_DATA;
        mlsNetworkHwWrite((UInt8 *)CHECK_EG95_SIGNAL_QUALITY, strlen(CHECK_EG95_SIGNAL_QUALITY));
        EG95_DEBUG_PRINTF(CHECK_EG95_SIGNAL_QUALITY);

        tickCount = mlsOsalGetTickCount();
        do
        {
            node = slist_SearchNode(gpNetRspList, RESP_OK, strlen(RESP_OK));
            if (Null != node)
            {
                EG95_DEBUG_PRINTF(RESP_OK"\r\n");
                slist_RemoveNode(gpNetRspList, node);
                errorCode = MLS_SUCCESS;
                break;
            }
            else
            {
                mlsOsalDelayMs(5);
            }
        }while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

        if (errorCode != MLS_SUCCESS)
        {
            break;
        }

        errorCode = MLS_ERR_NETWORK_NONE_DATA;
        tickCount = mlsOsalGetTickCount();
        do
        {
            node = slist_SearchNode(gpNetRspList, RESP_EG95_CSQ, strlen(RESP_EG95_CSQ));
            if (Null != node)
            {
                memset(buffer, 0, sizeof(buffer));
                memcpy(buffer, ((struct NodeData *)node->data)->data,
                        ((struct NodeData *)node->data)->len);
                EG95_DEBUG_PRINTF("%s\r\n", buffer);
                slist_RemoveNode(gpNetRspList, node);

                memcpy(rssi_str, &buffer[6], 2);
                *rssi = (UInt32)atoi(rssi_str);
                errorCode = MLS_SUCCESS;
                break;
            }
            else
            {
                mlsOsalDelayMs(5);
            }
        }while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);
    }while(0);

    if (errorCode == MLS_ERR_NETWORK_NONE_DATA)
    {
        EG95_DEBUG_PRINTF("[FAILED] Hang on get RSSI\r\n");
        gIsNetworkHang = True;
    }

    return errorCode;
}

mlsErrorCode_t mlsEG95GetSIMID(UInt8 *id, UInt8 *lenID)
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_NONE_DATA;
	struct Node *node = Null;
	UInt32 tickCount;
	char buffer[50];

	mlsNetworkHwWrite((UInt8 *)GET_EG95_SIM_ID, strlen(GET_EG95_SIM_ID));
#if DEBUG_LOG_NETWORK
	mlsDebugPrint(GET_EG95_SIM_ID);
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
			errorCode = MLS_SUCCESS;
			break;
		}
		else
		{
			errorCode = MLS_ERR_NETWORK_FAIL;
			node = slist_SearchNode(gpNetRspList, RESP_ERROR, strlen(RESP_ERROR));
			if (Null != node)
			{
				slist_RemoveNode(gpNetRspList, node);
				break;
			}
			mlsOsalDelayMs(5);
		}
	}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

	if (errorCode == MLS_SUCCESS)
	{
		tickCount = mlsOsalGetTickCount();
		do
		{
			node = slist_SearchNode(gpNetRspList, RESP_EG95_GET_SIM_ID, strlen(RESP_EG95_GET_SIM_ID));
			if (Null != node)
			{
				memset(buffer, 0, sizeof(buffer));
				memcpy(buffer, ((struct NodeData *)node->data)->data,
						((struct NodeData *)node->data)->len);
#if DEBUG_LOG_NETWORK
				mlsDebugPrint("%s\r\n", buffer);
#endif
				slist_RemoveNode(gpNetRspList, node);
				*lenID = strlen(buffer) - strlen(RESP_EG95_GET_SIM_ID);
				memcpy(&id[0], &buffer[strlen(RESP_EG95_GET_SIM_ID)], *lenID);
				errorCode = MLS_SUCCESS;
				break;
			}
			else
			{
				errorCode = MLS_ERR_NETWORK_NONE_DATA;
				mlsOsalDelayMs(5);
			}
		}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);
	}

	return errorCode;
}

/**@}*/
