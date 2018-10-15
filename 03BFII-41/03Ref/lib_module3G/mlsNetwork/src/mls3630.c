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
 * @defgroup mls3630.c	mls3630.c
 * @brief mls3630.c
 *
 * Add more details about module
 * @{
 */

/**
 * @file	mls3630.c
 *
 * @date 	8 Feb 2018
 * @author	stylvn008
 */


/********** Include section ***************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/mls3630.h"
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
}mls3630ProcessRsp_t;

/********** Local Macro definition section ************************************/

/********** Global variable definition section ********************************/

/********** Local (static) variable definition section ************************/
static mls3630ProcessRsp_t g3630ProcessRspState = PROCESS_START_CR;
static UInt16 g3630DataRspLen = 0;
static UInt8 _cntComma;
static UInt32 RecvLenData = 0;
static UInt32 g3630DataRspTickCount = 0;

/********** Local (static) function declaration section ***********************/
static inline Void mls3630ProcessRsp(UInt8 data);
/********** Local function definition section *********************************/
static inline Void mls3630ProcessRsp(UInt8 data)
{
	switch (g3630ProcessRspState)
	{
	case PROCESS_START_CR:
		if (AT_DATA_CR == data)
		{
			g3630ProcessRspState = PROCESS_START_LF;
		}
		break;
	case PROCESS_START_LF:
		if (AT_DATA_LF == data)
		{
			g3630ProcessRspState = PROCESS_DATA;
			g3630DataRspLen = 0;
		}
		break;
	case PROCESS_DATA:
		if (AT_DATA_CR != data)
		{
			gNetworkDataRsp[g3630DataRspLen] = data;
			g3630DataRspLen ++;
			if (AT_DATA_SEND == data)
			{
				g3630ProcessRspState = PROCESS_START_SEND;
			}
			if (0 == memcmp(gNetworkDataRsp, RESP_RECV, strlen(RESP_RECV)) &&
					g3630DataRspLen == strlen(RESP_RECV))
			{
			    _cntComma = 0;
				RecvLenData = 0;
				g3630DataRspTickCount = mlsOsalGetTickCount();
				g3630ProcessRspState = PROCESS_START_RECV;
			}
		}
		else
		{
			g3630ProcessRspState = PROCESS_END_LF;
		}
		break;
	case PROCESS_END_LF:
		if (AT_DATA_LF == data)
		{
			if (g3630DataRspLen != 0)
			{
				// Create a node
				struct Node *node = NodeCreate(gNetworkDataRsp, g3630DataRspLen);

				// Insert node to list
				if (Null != node)
				{
					slist_InsertLast(gpNetRspList, node);
				}

				g3630ProcessRspState = PROCESS_START_CR;
			}
			else
			{
				g3630ProcessRspState = PROCESS_DATA;
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

		g3630ProcessRspState = PROCESS_START_CR;
		break;
	case PROCESS_START_RECV:
		gNetworkDataRsp[g3630DataRspLen] = data;
		g3630DataRspLen ++;
		if ((',' == data) && (_cntComma < 4))
		{
		    _cntComma++;
			break;
		}
		if (_cntComma == 3)
		{
			RecvLenData = (RecvLenData * 10) + data - '0';
		}
		if (_cntComma == 4)
		{
			RecvLenData--;
			if (RecvLenData == 0)
			{
				g3630ProcessRspState = PROCESS_END_LF;
			}
		}
		if (mlsOsalGetElapseTime(g3630DataRspTickCount) > SOCKET_TIMEOUT_MS)
		{
			g3630ProcessRspState = PROCESS_START_LF;
		}
		break;
	default:
		break;
	}
}
/********** Global function definition section ********************************/

mlsErrorCode_t mls3630Init()
{
	mlsErrorCode_t errorCode = MLS_SUCCESS;

	/* Initialize network hardware */
	errorCode = mlsNetworkHwInit(mls3630ProcessRsp);

	if (MLS_SUCCESS != errorCode)
	{
		return errorCode;
	}

	return errorCode;
}

mlsErrorCode_t mls3630CheckReady()
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_FAIL;
	struct Node *node = Null;
	UInt32 tickCount;

	tickCount = mlsOsalGetTickCount();
	mlsNetworkHwWrite((UInt8 *)CHECK_3630_READY_CR_LF, strlen(CHECK_3630_READY_CR_LF));
#if DEBUG_LOG_NETWORK
	mlsDebugPrint(CHECK_3630_READY_CR_LF);
#endif
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
			mlsOsalDelayMs(5);
		}
	}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

	if (errorCode == MLS_SUCCESS)
	{
		errorCode = MLS_ERR_NETWORK_FAIL;

		tickCount = mlsOsalGetTickCount();
		mlsNetworkHwWrite((UInt8 *)DISABLE_3630_COMMAND_ECHO, strlen(DISABLE_3630_COMMAND_ECHO));
#if DEBUG_LOG_NETWORK
		mlsDebugPrint(DISABLE_3630_COMMAND_ECHO);
#endif
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
				mlsOsalDelayMs(5);
			}
		}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);
	}

	mlsNetworkNodeRemoveAll();

	return errorCode;
}

mlsErrorCode_t mls3630SetAPN(const char *pAPN)
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_FAIL;
	char buffer[50];
	struct Node *node = Null;
	UInt32 tickCount;

	tickCount = mlsOsalGetTickCount();
	sprintf(buffer, "AT+CGDCONT=1,\"IP\",\"%s\"\r\n", pAPN);
#if DEBUG_LOG_NETWORK
	mlsDebugPrint(buffer);
#endif
	mlsNetworkHwWrite((UInt8 *)buffer, (UInt16)strlen(buffer));

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
			node = slist_SearchNode(gpNetRspList, RESP_ERROR, strlen(RESP_ERROR));
			if (Null != node)
			{
				slist_RemoveNode(gpNetRspList, node);
				break;
			}
			mlsOsalDelayMs(5);
		}
	}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

	return errorCode;
}

mlsErrorCode_t mls3630Act()
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_FAIL;
	struct Node *node = Null;
	char buffer[50] = {0};
	UInt32 tickCount;

	mlsNetworkHwWrite((UInt8 *)QUERY_3630_NETWORK_STATE, strlen(QUERY_3630_NETWORK_STATE));
#if DEBUG_LOG_NETWORK
	mlsDebugPrint(QUERY_3630_NETWORK_STATE);
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

	gIsCheckDown = False;

	tickCount = mlsOsalGetTickCount();
	do
	{
		node = slist_SearchNode(gpNetRspList, "+ZIPCALL: 0", strlen("+ZIPCALL: 0"));
		if (Null != node)
		{
#if DEBUG_LOG_NETWORK
			mlsDebugPrint("+ZIPCALL: 0\r\n");
#endif
			slist_RemoveNode(gpNetRspList, node);

			mlsNetworkHwWrite((UInt8 *)ACTIVE_3630_NETWORK, strlen(ACTIVE_3630_NETWORK));
#if DEBUG_LOG_NETWORK
			mlsDebugPrint(ACTIVE_3630_NETWORK);
#endif
			errorCode = MLS_ERR_NETWORK_FAIL;
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
					mlsOsalDelayMs(5);
				}
			}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

			if (errorCode == MLS_SUCCESS)
			{
				errorCode = MLS_ERR_NETWORK_FAIL;
				tickCount = mlsOsalGetTickCount();
				do
				{
					node = slist_SearchNode(gpNetRspList, "+ZIPCALL: ", strlen("+ZIPCALL: "));
					if (Null != node)
					{
						memcpy(buffer, ((struct NodeData *)node->data)->data,
								((struct NodeData *)node->data)->len);
#if DEBUG_LOG_NETWORK
						mlsDebugPrint("%s\r\n", buffer);
#endif
						slist_RemoveNode(gpNetRspList, node);
						if (0 == memcmp((UInt8 *)"0", &buffer[strlen((char *)buffer)-1], 1))
						{
							errorCode = MLS_ERR_NETWORK_FAIL;
						}
						else
						{
							errorCode = MLS_SUCCESS;
						}
						break;
					}
					else
					{
						mlsOsalDelayMs(5);
					}
				}while(mlsOsalGetElapseTime(tickCount) < ESTABLISH_TCP_TIMEOUT_MS);
			}
			break;
		}
		else
		{
			node = slist_SearchNode(gpNetRspList, "+ZIPCALL: ", strlen("+ZIPCALL: "));
			if (Null != node)
			{
#if DEBUG_LOG_NETWORK
				memcpy(buffer, ((struct NodeData *)node->data)->data,
						((struct NodeData *)node->data)->len);
				mlsDebugPrint("%s\r\n", buffer);
#endif
				slist_RemoveNode(gpNetRspList, node);
				errorCode = MLS_SUCCESS;
				break;
			}
			mlsOsalDelayMs(5);
		}
	}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

	gIsCheckDown = True;

	return errorCode;
}

Int32 mls3630CreateSocket(Char *ip, Char *port)
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_NONE_DATA;
	char buffer[50] = {0};
	struct Node *node = Null;
	int sockID = 0;
	UInt32 tickCount;
	int i;

	for (i = 0; i < MAX_3630_SOCKET; i++)
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
#if DEBUG_LOG_NETWORK
		mlsDebugPrint("All socket are unavailable. Check socket again!\r\n");
#endif
		tickCount = mlsOsalGetTickCount();
		do
		{
			node = slist_SearchNode(gpNetRspList, "+", strlen("+"));
			if (Null != node)
			{
				if (0 == memcmp(((struct NodeData *)node->data)->data, RESP_RECV, strlen(RESP_RECV)))
				{
#if DEBUG_LOG_NETWORK
					char buff[50] = {0};
					memcpy(buff, ((struct NodeData *)node->data)->data, 50);
					mlsDebugPrint("Clear unused resp: %s. Length: %d\r\n", buff, ((struct NodeData *)node->data)->len);
#endif
				}
				else
				{
					for (i = MAX_3630_SOCKET - 1; i >= 0; i--)
					{
						sprintf(buffer, "+ZIPSTAT: %d,1", gSocketTable[i].sockID);
						if (0 == memcmp(((struct NodeData *)node->data)->data, buffer, strlen(buffer)))
						{
							gSocketTable[i].available = True;
							sockID = gSocketTable[i].sockID;
#if DEBUG_LOG_NETWORK
							mlsDebugPrint("Socket %d now available\r\n", sockID);
#endif
							break;
						}
					}
				}
				slist_RemoveNode(gpNetRspList, node);
			}
			else
			{
				break;
			}
		}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

		if (sockID > 0)
		{
			gSocketTable[sockID-1].available = False;
			gIsCheckClose = True;
			return sockID;
		}

		for (i = MAX_3630_SOCKET - 1; i >= 0; i--)
		{
			sprintf(buffer, "AT+ZIPSTAT=%d\r\n", gSocketTable[i].sockID);
			mlsNetworkHwWrite((UInt8 *)buffer, (UInt16)strlen(buffer));
#if DEBUG_LOG_NETWORK
			mlsDebugPrint(buffer);
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

			errorCode = MLS_ERR_NETWORK_NONE_DATA;
			tickCount = mlsOsalGetTickCount();
			do
			{
				sprintf(buffer, "+ZIPSTAT: %d,", gSocketTable[i].sockID);
				node = slist_SearchNode(gpNetRspList, buffer, (UInt16)strlen(buffer));
				if (Null != node)
				{
					memset(buffer, 0, sizeof(buffer));
					memcpy(buffer, ((struct NodeData *)node->data)->data,
							((struct NodeData *)node->data)->len);
#if DEBUG_LOG_NETWORK
					mlsDebugPrint("%s\r\n", buffer);
#endif
					slist_RemoveNode(gpNetRspList, node);

					if (0 == memcmp((UInt8 *)"0", &buffer[strlen((char *)buffer)-1], 1))
					{
						gSocketTable[i].available = True;
						sockID = gSocketTable[i].sockID;
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

			if (errorCode == MLS_ERR_NETWORK_NONE_DATA)
			{
				gIsNetworkHang = True;
			}
		}
	}

	if (sockID < 1)
	{
#if DEBUG_LOG_NETWORK
		mlsDebugPrint("No available socket\r\n");
#endif
		gIsCheckClose = True;
		return errorCode;
	}

//	errorCode = MLS_ERR_NETWORK_NONE_DATA;
	sprintf(buffer, "AT+ZIPOPEN=%d,%d,%s,%s\r\n", sockID, SOCKET_MODE, ip, port);
	mlsNetworkHwWrite((UInt8 *)buffer, (UInt16)strlen(buffer));
#if DEBUG_LOG_NETWORK
	mlsDebugPrint(buffer);
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
			node = slist_SearchNode(gpNetRspList, RESP_ERROR, (UInt16)strlen(RESP_ERROR));
			if (Null != node)
			{
#if DEBUG_LOG_NETWORK
				mlsDebugPrint(RESP_ERROR"\r\n");
#endif
				slist_RemoveNode(gpNetRspList, node);
				errorCode = MLS_ERR_NETWORK_FAIL;

				sprintf(buffer, "AT+ZIPSTAT=%d\r\n", sockID);
				mlsNetworkHwWrite((UInt8 *)buffer, (UInt16)strlen(buffer));
#if DEBUG_LOG_NETWORK
				mlsDebugPrint(buffer);
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

				break;
			}
			mlsOsalDelayMs(5);
		}
	}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

	if (errorCode != MLS_ERR_NETWORK_NONE_DATA)
	{
		errorCode = MLS_ERR_NETWORK_NONE_DATA;
		tickCount = mlsOsalGetTickCount();
		do
		{
			memset(buffer, 0, sizeof(buffer));
			sprintf(buffer, "+ZIPSTAT: %d,", sockID);
			node = slist_SearchNode(gpNetRspList, buffer, (UInt16)strlen(buffer));
			if (Null != node)
			{
				memset(buffer, 0, sizeof(buffer));
				memcpy(buffer, ((struct NodeData *)node->data)->data,
						((struct NodeData *)node->data)->len);
#if DEBUG_LOG_NETWORK
				mlsDebugPrint("%s\r\n", buffer);
#endif
				slist_RemoveNode(gpNetRspList, node);

				gSocketTable[sockID-1].available = False;
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
		}while(mlsOsalGetElapseTime(tickCount) < SOCKET_TIMEOUT_MS);
	}

	gIsCheckClose = True;

	if (errorCode == MLS_SUCCESS)
	{
		return sockID;
	}
	else if (errorCode == MLS_ERR_NETWORK_NONE_DATA)
	{
		gIsNetworkHang = True;
	}

	return errorCode;
}

mlsErrorCode_t mls3630WriteSocket(Int32 sockID, Char *buffer, UInt16 bufferLen)
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_NONE_DATA;
	char buffer_temp[50];
	struct Node *node = Null;
	UInt32 tickCount;

	if (gSocketTable[sockID-1].available == True)
	{
		return MLS_ERR_NETWORK_FAIL;
	}

	sprintf(buffer_temp, "AT+ZIPSENDRAW=%d,%d\r\n", (int)sockID, bufferLen);
	mlsNetworkHwWrite((UInt8 *)buffer_temp, (UInt16)strlen(buffer_temp));
#if DEBUG_LOG_NETWORK
	mlsDebugPrint(buffer_temp);
#endif
	tickCount = mlsOsalGetTickCount();
	do
	{
		node = slist_SearchNode(gpNetRspList, "\r\n> ", 4);
		if (Null != node)
		{
			slist_RemoveNode(gpNetRspList, node);
#if DEBUG_LOG_NETWORK
			mlsDebugPrint("\r\n> ");
#endif
			errorCode = MLS_SUCCESS;
			break;
		}
		else
		{
			node = slist_SearchNode(gpNetRspList, RESP_ERROR, strlen(RESP_ERROR));
			if (Null != node)
			{
#if DEBUG_LOG_NETWORK
				mlsDebugPrint(RESP_ERROR"\r\n");
#endif
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

		memset(buffer_temp, 0, sizeof(buffer_temp));
		sprintf(buffer_temp, RESP_SENDRAW, (int)sockID, bufferLen);
		tickCount = mlsOsalGetTickCount();
		do
		{
			node = slist_SearchNode(gpNetRspList, buffer_temp, strlen(buffer_temp));
			if (Null != node)
			{
#if DEBUG_LOG_NETWORK
				memset(buffer_temp, 0, sizeof(buffer_temp));
				memcpy(buffer_temp, ((struct NodeData *)node->data)->data,
						((struct NodeData *)node->data)->len);
				mlsDebugPrint("%s\r\n", buffer_temp);
#endif
				slist_RemoveNode(gpNetRspList, node);
				errorCode = MLS_SUCCESS;
				break;
			}
			else
			{
				mlsOsalDelayMs(5);
			}
		}while(mlsOsalGetElapseTime(tickCount) < SOCKET_TIMEOUT_MS);

		if (errorCode == MLS_SUCCESS)
		{
			errorCode = MLS_ERR_NETWORK_NONE_DATA;
			tickCount = mlsOsalGetTickCount();
			do
			{
				node = slist_SearchNode(gpNetRspList, RESP_OK, strlen(RESP_OK));
				if (Null != node)
				{
					slist_RemoveNode(gpNetRspList, node);
#if DEBUG_LOG_NETWORK
					mlsDebugPrint("\r\nOK Sent\r\n");
#endif
					errorCode = MLS_SUCCESS;
					break;
				}
				else
				{
					mlsOsalDelayMs(5);
				}
			}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);
		}
	}

	if ((errorCode == MLS_ERR_NETWORK_NONE_DATA) && (gSocketTable[sockID-1].available == False))
	{
		gIsNetworkHang = True;
	}

	return errorCode;
}

mlsErrorCode_t mls3630ReadSocket(Int32 sockID, Char *buffer, UInt16 *bufferLen)
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_NONE_DATA;
	struct Node *node = Null;
	char buffer_temp[AT_RESPONSE_SIZE] = {0};
	UInt32 tickCount;
	UInt8 cntComma = 0;
	UInt16 length = 0;
	UInt16 idx = 0;
	int i;

	sprintf(buffer_temp, "+ZIPRECV: %d", (int)sockID);
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
				if ((buffer_temp[i] == ',') && (cntComma < 4))
				{
					cntComma++;
					continue;
				}
				if (cntComma == 3)
				{
					length = (UInt16)((length * 10) + buffer_temp[i] - '0');
				}
				if (cntComma == 4)
				{
					buffer[idx++] = buffer_temp[i];
				}
			}
			*bufferLen = length;
			slist_RemoveNode(gpNetRspList, node);
#if DEBUG_LOG_NETWORK
			memset(buffer_temp, 0, sizeof(buffer_temp));
			sprintf(buffer_temp, "OK Received! Length: %d\r\n", (int)length);
			mlsDebugPrint(buffer_temp);
#endif
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
#if DEBUG_LOG_NETWORK
			mlsDebugPrint("mls3630ReadSocket socket closed\r\n");
#endif
			break;
		}
	}while(mlsOsalGetElapseTime(tickCount) < SOCKET_TIMEOUT_MS);

	if (errorCode == MLS_ERR_NETWORK_NONE_DATA)
	{
		gIsNetworkHang = True;
#if DEBUG_LOG_NETWORK
        mlsDebugPrint("mls3630ReadSocket hang\r\n");
#endif
	}

	return errorCode;
}

mlsErrorCode_t mls3630CloseSocket(Int32 sockID)
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_NONE_DATA;
	char buffer[50];
	struct Node *node = Null;
	UInt32 tickCount;

	gIsCheckClose = False;

	sprintf(buffer, "AT+ZIPCLOSE=%d\r\n", (int)sockID);
	mlsNetworkHwWrite((UInt8 *)buffer, (UInt16)strlen(buffer));
#if DEBUG_LOG_NETWORK
	mlsDebugPrint(buffer);
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
			node = slist_SearchNode(gpNetRspList, RESP_ERROR, strlen(RESP_ERROR));
			if (Null != node)
			{
#if DEBUG_LOG_NETWORK
				mlsDebugPrint(RESP_ERROR"\r\n");
#endif
				slist_RemoveNode(gpNetRspList, node);
				errorCode = MLS_ERR_NETWORK_FAIL;
				break;
			}
			mlsOsalDelayMs(5);
		}
	}while(mlsOsalGetElapseTime(tickCount) < AT_COMMAND_TIMEOUT_MS);

	if (errorCode == MLS_SUCCESS)
	{
		errorCode = MLS_ERR_NETWORK_FAIL;
		tickCount = mlsOsalGetTickCount();
		do
		{
			sprintf(buffer, "+ZIPSTAT: %d,", (int)sockID);
			node = slist_SearchNode(gpNetRspList, buffer, (UInt16)strlen(buffer));
			if (Null != node)
			{
				memset(buffer, 0, sizeof(buffer));
				memcpy(buffer, ((struct NodeData *)node->data)->data,
						((struct NodeData *)node->data)->len);
#if DEBUG_LOG_NETWORK
				mlsDebugPrint("%s\r\n", buffer);
#endif
				slist_RemoveNode(gpNetRspList, node);
				if (0 == memcmp((UInt8 *)"0", &buffer[strlen((char *)buffer)-1], 1))
				{
					gSocketTable[sockID-1].available = True;
					errorCode = MLS_SUCCESS;
				}
				else
				{
					gSocketTable[sockID-1].available = False;
					errorCode = MLS_ERR_NETWORK_FAIL;
				}

				break;
			}
			else
			{
				mlsOsalDelayMs(5);
			}
		}while(mlsOsalGetElapseTime(tickCount) < SOCKET_CLOSE_TIMEOUT_MS);
	}

	mlsNetworkNodeRemoveAll();

	if (errorCode == MLS_ERR_NETWORK_NONE_DATA)
	{
		gIsNetworkHang = True;
	}

	gIsCheckClose = True;

	return errorCode;
}

mlsErrorCode_t mls3630GetRSSI(UInt32 *rssi)
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_NONE_DATA;
	char buffer[50];
	char rssi_str[2];
	struct Node *node = Null;
	UInt32 tickCount;
	*rssi = RSSI_VALUE_NO_NETWORK;

	do
	{
        mlsNetworkHwWrite((UInt8 *)CHECK_3630_SIM_STATUS, strlen(CHECK_3630_SIM_STATUS));
#if DEBUG_LOG_NETWORK
        mlsDebugPrint(CHECK_3630_SIM_STATUS);
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
            node = slist_SearchNode(gpNetRspList, RESP_3630_CPIN_READY, strlen(RESP_3630_CPIN_READY));
            if (Null != node)
            {
#if DEBUG_LOG_NETWORK
                mlsDebugPrint(RESP_3630_CPIN_READY"\r\n");
#endif
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
        mlsNetworkHwWrite((UInt8 *)NETWORK_CREG_REGISTRATION, strlen(NETWORK_CREG_REGISTRATION));
#if DEBUG_LOG_NETWORK
        mlsDebugPrint(NETWORK_CREG_REGISTRATION);
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
#if DEBUG_LOG_NETWORK
                mlsDebugPrint("%s\r\n", buffer);
#endif
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
        mlsNetworkHwWrite((UInt8 *)NETWORK_CEREG_REGISTRATION, strlen(NETWORK_CEREG_REGISTRATION));
#if DEBUG_LOG_NETWORK
        mlsDebugPrint(NETWORK_CEREG_REGISTRATION);
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
#if DEBUG_LOG_NETWORK
                mlsDebugPrint("%s\r\n", buffer);
#endif
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
        mlsNetworkHwWrite((UInt8 *)CHECK_3630_SIGNAL_QUALITY, strlen(CHECK_3630_SIGNAL_QUALITY));
#if DEBUG_LOG_NETWORK
        mlsDebugPrint(CHECK_3630_SIGNAL_QUALITY);
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
            node = slist_SearchNode(gpNetRspList, RESP_3630_CSQ, strlen(RESP_3630_CSQ));
            if (Null != node)
            {
                memset(buffer, 0, sizeof(buffer));
                memcpy(buffer, ((struct NodeData *)node->data)->data,
                        ((struct NodeData *)node->data)->len);
#if DEBUG_LOG_NETWORK
                mlsDebugPrint("%s\r\n", buffer);
#endif
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
		gIsNetworkHang = True;
	}

	return errorCode;
}

mlsErrorCode_t mls3630GetSIMID(UInt8 *id, UInt8 *lenID)
{
	mlsErrorCode_t errorCode = MLS_ERR_NETWORK_NONE_DATA;
	struct Node *node = Null;
	UInt32 tickCount;
	char buffer[50];

	mlsNetworkHwWrite((UInt8 *)GET_3630_SIM_ID, strlen(GET_3630_SIM_ID));
#if DEBUG_LOG_NETWORK
	mlsDebugPrint(GET_3630_SIM_ID);
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
			node = slist_SearchNode(gpNetRspList, RESP_CME_ERROR, strlen(RESP_CME_ERROR));
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
			node = slist_SearchNode(gpNetRspList, RESP_3630_GET_SIM_ID, strlen(RESP_3630_GET_SIM_ID));
			if (Null != node)
			{
				memset(buffer, 0, sizeof(buffer));
				memcpy(buffer, ((struct NodeData *)node->data)->data,
						((struct NodeData *)node->data)->len);
#if DEBUG_LOG_NETWORK
				mlsDebugPrint("%s\r\n", buffer);
#endif
				slist_RemoveNode(gpNetRspList, node);
				*lenID = strlen(buffer) - strlen(RESP_3630_GET_SIM_ID);
				memcpy(&id[0], &buffer[strlen(RESP_3630_GET_SIM_ID)], *lenID);
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
