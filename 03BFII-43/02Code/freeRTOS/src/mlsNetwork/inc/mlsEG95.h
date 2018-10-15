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
 * @defgroup mlsEG95.h	mlsEG95.h
 * @brief mlsEG95.h
 *
 * Add more details about module
 * @{
 */

/**
 * @file	mlsEG95.h
 *
 * @date 	15 Jun 2018
 * @author	sirius
 */

#ifndef SOURCE_STYL_MLSNETWORK_INC_MLSEG95_H_
#define SOURCE_STYL_MLSNETWORK_INC_MLSEG95_H_

#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ***************************************************/
#include "mlsInclude.h"
/********** Constant  and compile switch definition section *******************/
#define AT_DATA_CR		'\r'
#define AT_DATA_LF		'\n'
#define AT_DATA_SEND	'>'
#define AT_CR_LF		"\r\n"
#define AT_RESPONSE_PER_BYTE_TIMEOUT_MS	300

#define CHECK_EG95_READY				"AT"
#define CHECK_EG95_READY_CR_LF			"AT\r\n"
#define ENABLE_EG95_COMMAND_ECHO		"ATE1\r\n"
#define DISABLE_EG95_COMMAND_ECHO		"ATE0\r\n"
#define CHECK_EG95_SIM_STATUS           "AT+CPIN?\r\n"
#define EG95_CREG_REGISTRATION          "AT+CREG?\r\n"
#define EG95_CEREG_REGISTRATION         "AT+CEREG?\r\n"
#define CHECK_EG95_SIGNAL_QUALITY       "AT+CSQ\r\n"
#define GET_EG95_SIM_ID					"AT+QCCID\r\n"

#define SET_APN_EG95                    "AT+QICSGP=%d,%d,\"%s\"\r\n"
#define QUERY_EG95_NETWORK_STATE		"AT+QIACT?\r\n"
#define ACTIVE_EG95_NETWORK				"AT+QIACT=%d\r\n"
#define OPEN_SOCKET_EG95                "AT+QIOPEN=%d,%d,\"%s\",\"%s\",%s,0,%d\r\n"
#define QUERY_SOCKET_STATE              "AT+QISTATE=%d,%d\r\n"
#define SOCKET_SEND_DATA                "AT+QISEND=%d,%d\r\n"
#define CLOSE_SOCKET_EG95               "AT+QICLOSE=%d,%d\r\n"

#define TIMEOUT_EG95_BASE_MS            10000
#define CHECK_EG95_READY_TIMEOUT_MS		300
#define AT_COMMAND_TIMEOUT_MS			2000
#define CPIN_READY_TIMEOUT_MS           TIMEOUT_EG95_BASE_MS + 5000
#define ACTIVE_EG95_TIMEOUT_MS       	10000
#define SOCKET_EG95_TIMEOUT_MS			TIMEOUT_EG95_BASE_MS + 10000
#define SOCKET_EG95_OPEN_TIMEOUT_MS     2000
#define SOCKET_CLOSE_TIMEOUT_SEC        1
#define SOCKET_EG95_CLOSE_TIMEOUT_MS    SOCKET_CLOSE_TIMEOUT_SEC*1000 + 100

#define RESP_OK							"OK"
#define RESP_ERROR						"ERROR"
#define RESP_EG95_READY                 "RDY"
#define RESP_EG95_RECV					"+QIURC: \"recv\""
#define RESP_EG95_RECV_SOCKID           "+QIURC: \"recv\",%d"
#define RESP_EG95_CLOSED                "+QIURC: \"closed\",%d"
#define RESP_EG95_ACT                   "+QIACT: %d,%d,%d"
#define RESP_EG95_NETWORK_DOWN          "+QIURC: \"pdpdeact\",%d"
#define RESP_EG95_OPEN_SOCKET           "+QIOPEN: %d,"
#define RESP_EG95_QUERY_STATE           "+QISTATE: %d"
#define RESP_EG95_SEND_OK               "SEND OK"
#define RESP_EG95_SEND_FAIL             "SEND FAIL"

#define RESP_EG95_CPIN_READY			"+CPIN: READY"
#define RESP_CREG_REGISTERED			"+CREG: 0,"
#define RESP_CEREG						"+CEREG: 0,"
#define RESP_EG95_CSQ					"+CSQ: "
#define RESP_EG95_GET_SIM_ID			"+QCCID: "
#define RSSI_VALUE_NO_NETWORK			(99)

#define CTX_ID                          1
#define CTX_TYPE                        IPV4
#define SOCKET_EG95_MODE				"TCP"
#define ACCESS_MODE                     DIRECT_PUSH
#define QUERY_TYPE                      SPECIFIED_SOCKET
#define MAX_EG95_SOCKET                 11

/********** Type definition section *******************************************/
typedef enum
{
    DEACTIVATED = 0,
    ACTIVATED
}ContextState_t;

typedef enum
{
    IPV4 = 1,
    IPV6
}ContextType_t;

typedef enum
{
    BUFFER_ACCESS = 0,
    DIRECT_PUSH,
    TRANSPARENT_ACCESS
}EG95AccessMode_t;

typedef enum
{
    SPECIFIED_CONTEXT = 0,
    SPECIFIED_SOCKET
}QueryType_t;

typedef enum
{
    INITIAL = 0,
    OPENING,
    CONNECTED,
    LISTENING,
    CLOSING,
}SocketState_t;

typedef enum
{
    RECV_ID = 1,
    RECV_LEN,
    RECV_DATA = 4,
}RecvPart_t;
/********** Macro definition section*******************************************/

/********** Function declaration section **************************************/

/**
 * @brief This function is to initialize network module
 * @return error code.
 */
mlsErrorCode_t mlsEG95Init();

/**
 * @brief This function is to check device ready or not
 * @return error code
 */
mlsErrorCode_t mlsEG95CheckReady();

/**
 * @brief This function is to configure network APN
 * @param[in, out] pParam is pointer to parameter input
 * @return error code
 */
mlsErrorCode_t mlsEG95SetAPN(const char *pAPN);

/**
 * @brief This function is to connect device to 3G/4G network
 * @param[in, out] pParam is pointer to parameter input
 * @return error code
 */
mlsErrorCode_t mlsEG95Act();

/**
 * @brief This function is to create a socket connection for application
 * @param[in] ip is pointer to IP address
 * @param[in] port is pointer to port
 * @return error code or Socket ID if success
 */
Int32 mlsEG95CreateSocket(Char *ip, Char *port);

/**
 * @brief This function is to write data to target socket
 * @param[in] sockID is socket ID
 * @param[in] buffer is pointer to buffer input
 * @param[in] bufferLen is buffer input length
 * @return error code
 */
mlsErrorCode_t mlsEG95WriteSocket(Int32 sockID, Char *buffer, UInt16 bufferLen);

/**
 * @brief This function is to read data from target socket
 * @param[in] sockID is socket ID
 * @param[out] buffer is pointer to buffer output
 * @param[out] bufferLen is pointer to buffer output length
 * @return error code
 */
mlsErrorCode_t mlsEG95ReadSocket(Int32 sockID, Char *buffer, UInt16 *bufferLen);

/**
 * @brief This function is to close the socket which open before
 * @param[in] sockID is socket ID
 * @return error code
 */
mlsErrorCode_t mlsEG95CloseSocket(Int32 sockID);

/**
 * @brief This function is to get RSSI at current
 * @param[out] rssi is pointer to RSSI
 * @return error code
 */
mlsErrorCode_t mlsEG95GetRSSI(UInt32 *rssi);

mlsErrorCode_t mlsEG95GetSIMID(UInt8 *id, UInt8 *lenID);

#ifdef __cplusplus
}
#endif

#endif /* SOURCE_STYL_MLSNETWORK_INC_MLSEG95_H_ */

/**@}*/
