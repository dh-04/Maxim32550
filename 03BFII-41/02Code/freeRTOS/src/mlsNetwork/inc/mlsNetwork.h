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
 * @defgroup mlsNetworkAbstract.h	mlsNetworkAbstract.h
 * @brief mlsNetworkAbstract.h
 *
 * Add more details about module
 * @{
 */

/**
 * @file	mlsNetworkAbstract.h
 *
 * @date 	8 Feb 2018
 * @author	stylvn008
 */

#ifndef SOURCE_STYL_MLSNETWORK_INC_MLSNETWORK_H_
#define SOURCE_STYL_MLSNETWORK_INC_MLSNETWORK_H_


#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ***************************************************/
#include "mlsInclude.h"
/********** Constant  and compile switch definition section *******************/
#define CHECK_MODEL_ID          "AT+CGMM\r\n"
#define MODULE_ME3630           "ME3630"
#define MODULE_EG95             "EG95"
#define MAX_SOCKET_ID           11
#define AT_RESPONSE_SIZE		1024 * 5

/********** Type definition section *******************************************/
typedef struct
{
	Bool available;
	int sockID;
	Char ip[30];
	Char port[6];
	Char localPort[6];
}mlsSocketInfo_t;

typedef enum
{
    ZTE_ME3630,
    EG95,
}mlsNetworkModuleType_t;

typedef struct
{
    mlsErrorCode_t (*pFuncInit)(void);
    mlsErrorCode_t (*pFuncCheckReady)(void);
    mlsErrorCode_t (*pFuncSetAPN)(const char *pAPN);
    mlsErrorCode_t (*pFuncAct)(void);
    Int32 (*pFuncCreateSocket)(Char *ip, Char *port);
    mlsErrorCode_t (*pFuncWriteSocket)(Int32 sockID, Char *buffer, UInt16 bufferLen);
    mlsErrorCode_t (*pFuncReadSocket)(Int32 sockID, Char *buffer, UInt16 *bufferLen);
    mlsErrorCode_t (*pFuncCloseSocket)(Int32 sockID);
    mlsErrorCode_t (*pFuncGetRSSI)(UInt32 *rssi);
    mlsErrorCode_t (*pFuncGetSIMID)(UInt8 *id, UInt8 *lenID);
}mlsNetworkFunction_t;
/********** Macro definition section*******************************************/

/********** Function declaration section **************************************/
struct SList *gpNetRspList;
extern Bool gIsCheckDown;
extern Bool gIsNetworkHang;
extern Bool gIsCheckClose;
extern Bool gIsChangedSIM;
extern mlsSocketInfo_t gSocketTable[MAX_SOCKET_ID];
extern mlsNetworkModuleType_t gModule;
extern UInt8 gNetworkDataRsp[AT_RESPONSE_SIZE];

/**
 * @brief This function is to check which module is used
 * @return error code
 */
mlsErrorCode_t mlsNetworkCheckModule(mlsNetworkModuleType_t *pModule);

/**
 * @brief This function is to check device ready or not
 * @return error code
 */
mlsErrorCode_t mlsNetworkCheckReady();

/**
 * @brief This function is to configure network
 * @param[in, out] pParam is pointer to parameter input
 * @return error code
 */
mlsErrorCode_t mlsNetworkSetAPN(const char *pAPN);

/**
 * @brief This function is to connect device to 3G/4G network
 * @param[in, out] pParam is pointer to parameter input
 * @return error code
 */
mlsErrorCode_t mlsNetworkAct();

/**
 * @brief This function is to initialize network module
 * @return error code.
 */
mlsErrorCode_t mlsNetworkInit();

/**
 * @brief This function is to create a socket connection for application
 * @param[in] ip is pointer to IP address
 * @param[in] port is pointer to port
 * @return socket ID. return MLS_ERROR or 0 if can not open. Socket ID is greater than 0
 */
Int32 mlsNetworkCreateSocket(Char *ip, Char *port);

/**
 * @brief This function is to write data to target socket
 * @param[in] sockID is socket ID
 * @param[in] buffer is pointer to buffer input
 * @param[in] bufferLen is buffer input length
 * @return error code
 */
mlsErrorCode_t mlsNetworkWriteSocket(Int32 sockID, Char *buffer, UInt16 bufferLen);

/**
 * @brief This function is to read data from target socket
 * @param[in] sockID is socket ID
 * @param[out] buffer is pointer to buffer output
 * @param[out] bufferLen is pointer to buffer output length
 * @return error code
 */
mlsErrorCode_t mlsNetworkReadSocket(Int32 sockID, Char *buffer, UInt16 *bufferLen);

/**
 * @brief This function is to close the socket which open before
 * @param[in] sockID is socket ID
 * @return error code
 */
mlsErrorCode_t mlsNetworkCloseSocket(Int32 sockID);

/**
 * @brief This function is to get RSSI at current
 * @param[out] rssi is pointer to rssi
 * @return error code
 */
mlsErrorCode_t mlsNetworkGetRSSI(UInt32 *rssi);

/**
 * @brief This function is to check Network ready or not
 * @return True/False
 */
Bool mlsNetworkIsReady();

/**
 * @brief This function is to set function pointer
 * @param[in] module is pointer to current module
 * @return error code
 */
mlsErrorCode_t mlsNetworkSetFunc(mlsNetworkModuleType_t module);

/**
 * @brief This function is to do unit test of network
 * @return error code.
 */
mlsErrorCode_t mlsNetworkTest();

/**
 * @brief This function is to do test socket
 * @return error code
 */
mlsErrorCode_t mlsNetworkSocketTest();

/**
 * @brief This function is to do test ISO8583
 * @return error code
 */
mlsErrorCode_t mlsNetworkISO8583Test();

/**
 * @brief This function is to do test http
 * @return error code
 */
mlsErrorCode_t mlsNetworkHttpTest();

/**
 * @brief This function is to do test https
 * @return error code
 */
mlsErrorCode_t mlsNetworkHttpsTest();

mlsErrorCode_t mlsNetworkNodeRemoveAll();

mlsErrorCode_t mlsNetworkGetTime(UInt32 *utc);

#ifdef __cplusplus
}
#endif

#endif /* SOURCE_STYL_MLSNETWORK_INC_MLSNETWORK_H_ */

/**@}*/
