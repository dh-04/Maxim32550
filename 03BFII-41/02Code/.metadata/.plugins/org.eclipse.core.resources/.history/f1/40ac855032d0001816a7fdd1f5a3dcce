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
 * @defgroup mlsHttps.c	mlsHttps.c
 * @brief mlsHttps.c
 *
 * Add more details about module
 * @{
 */

/**
 * @file	mlsHttps.c
 *
 * @date 	30 Apr 2018
 * @author	stylvn008
 */


/********** Include section ***************************************************/
#include <string.h>
#include "../../mlsHTTPS/inc/mlsHTTPS.h"
#include "../mlsNetwork/inc/mlsNetwork.h"
#include "../mlsDebug/inc/mlsDebug.h"
#include "../mlsOsal/inc/mlsOsal.h"
#include "../../wolfssl/inc/options.h"
#include "../../wolfssl/inc/ssl.h"
#include "../../wolfssl/inc/wolfcrypt/memory.h"
#include "../mlsNETSPure20/mlsNETSPure20.h"
/********** Local Constant and compile switch definition section **************/
#if DEBUG_LOG_NETWORK
#define HTTPS_DEBUG_PRINTF(...)		mlsDebugPrint(__VA_ARGS__)
#else
#define HTTPS_DEBUG_PRINTF(...)
#endif

/********** Local Type definition section *************************************/

/********** Local Macro definition section ************************************/

/********** Global variable definition section ********************************/
char bufferData[HTTPS_RESP_SIZE];

/********** Local (static) variable definition section ************************/

/********** Local (static) function declaration section ***********************/

/********** Local function definition section *********************************/
static UInt8 gSocketRcvBuffer[1024*5];
static UInt16 gSocketRcvBufferLen;
static UInt16 gSocketRcvBufferIdx;
static Bool gSocketRcvStream = False;
extern Bool gUpdateRssi;
mlsMutexHandle_t gSslMutex;

int httpsClientRecv(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
	int sockfd = *(int*)ctx;
	UInt32 copyLen = 0;

	if (gTxnCancelStart)
	{
		HTTPS_DEBUG_PRINTF("CANCEL socket read\n\r");
		return -1;
	}

	if (False == gSocketRcvStream)
	{
		mlsErrorCode_t errorCode;

		errorCode = mlsNetworkReadSocket(sockfd, (Char *)gSocketRcvBuffer, &gSocketRcvBufferLen);
		if ((MLS_SUCCESS == errorCode) && (gSocketRcvBufferLen > 0))
		{
			gSocketRcvBufferIdx = 0;
			gSocketRcvStream = True;
		}
		else
		{
			HTTPS_DEBUG_PRINTF("socket read fail\n\r");
			return -1;
		}
	}

	copyLen = MLS_MACRO_MIN(sz, gSocketRcvBufferLen - gSocketRcvBufferIdx);

	memcpy(buf, &gSocketRcvBuffer[gSocketRcvBufferIdx], copyLen);
	gSocketRcvBufferIdx += copyLen;

	if (gSocketRcvBufferIdx >= gSocketRcvBufferLen)
	{
		gSocketRcvStream = False;
	}

	HTTPS_DEBUG_PRINTF("clientRecv: %d\n\r", copyLen);

	return copyLen;
}

int httpsClientSend (WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
	mlsErrorCode_t errorCode = MLS_SUCCESS;
	int sockfd = *(int*)ctx;

	gSocketRcvStream = False;

	if (gTxnCancelStart)
	{
		HTTPS_DEBUG_PRINTF("CANCEL socket write\n\r");
		return -1;
	}

	errorCode = mlsNetworkWriteSocket(sockfd, buf, sz);
	if (errorCode != MLS_SUCCESS)
	{
		HTTPS_DEBUG_PRINTF("socket write fail\n\r");
		return -1;
	}

	HTTPS_DEBUG_PRINTF("clientSend: %d\n\r", sz);

	return sz;
}

Bool myStrStr(char *src, char *des)
{
	int idx = 0;
	Bool retVal = False;

	while (idx < strlen(des))
	{
		if (0 == memcmp(src, des + idx, MLS_MACRO_MIN(strlen(src), strlen(des))))
		{
			retVal = True;
			break;
		}
		else
		{
			idx ++;
		}
	}

	return retVal;
}
/********** Global function definition section ********************************/

mlsErrorCode_t mlsHttpsTransfer(char *caCertTypePem, char *ip, char *port,
								char *httpRequest, char *httpsResponse,
								int responseSize, int timeOutMs)
{
	enum {HTTP_PARSE_START, HTTP_PARSE_PROCCESS};
	mlsErrorCode_t errorCode;
	int sockfd;
	int len;
	char *pData = httpsResponse;
	int posStart;
	int index;
	int pattern;
	int BlacketCounter;
	int isProccessing;
	UInt32 retryTransferTick;

	/* declare wolfSSL objects */
	WOLFSSL_CTX* ctx;
	WOLFSSL*     ssl;

	retryTransferTick = mlsOsalGetTickCount();
	do
	{
		errorCode = MLS_SUCCESS;
		posStart = 0;
		index = 0;
		pattern = HTTP_PARSE_START;
		BlacketCounter = 0;
		isProccessing = 1;

		while (!mlsNetworkIsReady())
		{
			mlsOsalDelayMs(100);
		}

		gUpdateRssi = False;

		mlsOsalMutexLock(&gSslMutex, MLS_OSAL_MAX_DELAY);

		do
		{
			/* Create a socket that uses an Internet IPv4 address,
			 * Sets the socket to be stream based (TCP),
			 * 0 means choose the default protocol. */
			while(True)
			{
				if (gTxnCancelStart)
				{
					errorCode = PURE_ERROR_TXN_CANCELED;
					break;
				}

				sockfd = mlsNetworkCreateSocket(ip, port);

				if (sockfd > 0)
				{
					break;
				}
				else
				{
					mlsOsalDelayMs(2000);
					HTTPS_DEBUG_PRINTF("[FAILED] Retry create socket\r\n");
				}

			}

			if (errorCode == PURE_ERROR_TXN_CANCELED)
			{
				break;
			}

			if (sockfd < 0)
			{
				HTTPS_DEBUG_PRINTF("Create socket fail!\r\n");
				errorCode = MLS_ERROR;
				break;
			}

			HTTPS_DEBUG_PRINTF("Create socket success!\r\n");

			/******************************SSL/TLS Start**************************************/
			wolfSSL_SetAllocators(pvPortMalloc, vPortFree, pvPortRealloc);

			/* Initialize wolfSSL */
			wolfSSL_Init();

			/* Create and initialize WOLFSSL_CTX */
			if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method())) == NULL)
			{
				HTTPS_DEBUG_PRINTF("wolfSSL_CTX_new fail!\r\n");
				errorCode = MLS_ERROR;
				break;
			}

			HTTPS_DEBUG_PRINTF("wolfSSL_CTX_new success!\r\n");

			if (caCertTypePem == Null)
			{
				wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);
			}
			else
			{
				/* Load client certificates into WOLFSSL_CTX */
				if (wolfSSL_CTX_load_verify_buffer(ctx,
													(const unsigned char *)caCertTypePem,
													strlen(caCertTypePem),
													SSL_FILETYPE_PEM) != SSL_SUCCESS)
				{
					HTTPS_DEBUG_PRINTF("wolfSSL_CTX_load_verify_buffer fail!\r\n");
					errorCode = MLS_ERROR;
					break;
				}

				HTTPS_DEBUG_PRINTF("wolfSSL_CTX_load_verify_buffer success!\r\n");
			}

			/* Register callbacks */
			wolfSSL_SetIORecv(ctx, httpsClientRecv);
			wolfSSL_SetIOSend(ctx, httpsClientSend);


			/* Create a WOLFSSL object */
			if ((ssl = wolfSSL_new(ctx)) == NULL)
			{
				HTTPS_DEBUG_PRINTF("wolfSSL_new fail!\r\n");
				errorCode = MLS_ERROR;
				break;
			}

			HTTPS_DEBUG_PRINTF("wolfSSL_new success!\r\n");

			/* Attach wolfSSL to the socket */
			wolfSSL_set_fd(ssl, sockfd);

			/* Connect to wolfSSL on the server side */
			if (wolfSSL_connect(ssl) != SSL_SUCCESS)
			{
				HTTPS_DEBUG_PRINTF("wolfSSL_connect fail!\r\n");
				errorCode = MLS_ERROR;
				break;
			}
			HTTPS_DEBUG_PRINTF("wolfSSL_connect success!\r\n");

			/* Get a message for the server */
			HTTPS_DEBUG_PRINTF("HTTP request: \r\n%s\r\n", httpRequest);

			len = strlen(httpRequest);

			/* Send the message to the server */
			if (wolfSSL_write(ssl, httpRequest, strlen(httpRequest)) != len)
			{
				HTTPS_DEBUG_PRINTF("wolfSSL_write fail!\r\n");
				errorCode = MLS_ERROR;
				break;
			}

			HTTPS_DEBUG_PRINTF("wolfSSL_write success!\r\n");

			/* Read the server data into our buff array */
			memset(httpsResponse, 0, responseSize);

			while(isProccessing)
			{
				len = wolfSSL_read(ssl, pData, responseSize-1-posStart);
				if(len > 0)
				{
					for(index = 0; index < len && isProccessing == 1; index++)
					{
						switch(pattern)
						{
						case HTTP_PARSE_START:
							BlacketCounter = 0;
							if(httpsResponse[posStart + index] == '{')
							{
								BlacketCounter++;
								pattern = HTTP_PARSE_PROCCESS;
							}
							break;
						case HTTP_PARSE_PROCCESS:
							if(httpsResponse[posStart + index] == '{')
							{
								BlacketCounter++;
							}
							else if(httpsResponse[posStart + index] == '}')
							{
								BlacketCounter--;
								if(BlacketCounter == 0)
								{
									isProccessing = 0;
								}
							}
							break;
						default:
							break;
						}
					}

					pData += len;
					posStart += len;
				}
				else if(len < 0)
				{
					isProccessing = 0;
				}
			}

			/* Print to stdout any data the server sends */
			HTTPS_DEBUG_PRINTF("HTTPS response: %s\n", httpsResponse);

		}while(0);

		if (sockfd > 0)
		{
			/* Cleanup and return */
			if (ssl)
			{
				wolfSSL_free(ssl);      		/* Free the wolfSSL object                  */
				HTTPS_DEBUG_PRINTF("\r\nSSL free\r\n");
			}

			if (ctx)
			{
				wolfSSL_CTX_free(ctx);  		/* Free the wolfSSL context object          */
				HTTPS_DEBUG_PRINTF("SSL_CTX free\r\n");
			}

			wolfSSL_Cleanup();      			/* Cleanup the wolfSSL environment          */
			HTTPS_DEBUG_PRINTF("SSL_Cleanup success\r\n");

			if (gTxnCancelStart)
			{
				errorCode = PURE_ERROR_TXN_CANCELED;
			}
			else
			{
				mlsNetworkCloseSocket(sockfd);		/* Close the connection to the server       */
			}
		}

		mlsOsalMutexUnlock(&gSslMutex);

		gUpdateRssi = True;
		/******************************SSL/TLS End**************************************/
	}while((errorCode == MLS_ERROR) && (mlsOsalGetElapseTime(retryTransferTick) < timeOutMs));

	return errorCode;
}

mlsErrorCode_t mlsHttpsNFPSettlementTransfer(char *caCertTypePem, char *ip, char *port,
								char *httpRequest, char *httpsResponse,
								int responseSize, int timeOutMs)
{
	mlsErrorCode_t errorCode = MLS_SUCCESS;
	int sockfd;
	int len;
	int index = 0;
	char *pData = httpsResponse;

	/* declare wolfSSL objects */
	WOLFSSL_CTX* ctx;
	WOLFSSL*     ssl;

	while (!mlsNetworkIsReady())
	{
		mlsOsalDelayMs(100);
	}

	gUpdateRssi = False;

	do
	{
		/* Create a socket that uses an Internet IPv4 address,
		 * Sets the socket to be stream based (TCP),
		 * 0 means choose the default protocol. */
		if (gTxnCancelStart)
		{
			errorCode = PURE_ERROR_TXN_CANCELED;
			break;
		}

		if ((sockfd = mlsNetworkCreateSocket(ip, port)) < 0)
		{
			HTTPS_DEBUG_PRINTF("Create socket fail!\r\n");
			errorCode = MLS_ERROR;
			break;
		}

		HTTPS_DEBUG_PRINTF("Create socket success!\r\n");

		/******************************SSL/TLS Start**************************************/
		mlsOsalMutexLock(&gSslMutex, MLS_OSAL_MAX_DELAY);

		wolfSSL_SetAllocators(pvPortMalloc, vPortFree, pvPortRealloc);

		/* Initialize wolfSSL */
		wolfSSL_Init();

		/* Create and initialize WOLFSSL_CTX */
		if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method())) == NULL)
		{
			HTTPS_DEBUG_PRINTF("wolfSSL_CTX_new fail!\r\n");
			errorCode = MLS_ERROR;
			break;
		}

		HTTPS_DEBUG_PRINTF("wolfSSL_CTX_new success!\r\n");

		if (caCertTypePem == Null)
		{
			wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);
		}
		else
		{
			/* Load client certificates into WOLFSSL_CTX */
			if (wolfSSL_CTX_load_verify_buffer(ctx,
												(const unsigned char *)caCertTypePem,
												strlen(caCertTypePem),
												SSL_FILETYPE_PEM) != SSL_SUCCESS)
			{
				HTTPS_DEBUG_PRINTF("wolfSSL_CTX_load_verify_buffer fail!\r\n");
				errorCode = MLS_ERROR;
				break;
			}

			HTTPS_DEBUG_PRINTF("wolfSSL_CTX_load_verify_buffer success!\r\n");
		}

		/* Register callbacks */
		wolfSSL_SetIORecv(ctx, httpsClientRecv);
		wolfSSL_SetIOSend(ctx, httpsClientSend);


		/* Create a WOLFSSL object */
		if ((ssl = wolfSSL_new(ctx)) == NULL)
		{
			HTTPS_DEBUG_PRINTF("wolfSSL_new fail!\r\n");
			errorCode = MLS_ERROR;
			break;
		}

		HTTPS_DEBUG_PRINTF("wolfSSL_new success!\r\n");

		/* Attach wolfSSL to the socket */
		wolfSSL_set_fd(ssl, sockfd);

		/* Connect to wolfSSL on the server side */
		if (wolfSSL_connect(ssl) != SSL_SUCCESS)
		{
			HTTPS_DEBUG_PRINTF("wolfSSL_connect fail!\r\n");
			errorCode = MLS_ERROR;
			break;
		}
		HTTPS_DEBUG_PRINTF("wolfSSL_connect success!\r\n");

		/* Get a message for the server */
		HTTPS_DEBUG_PRINTF("HTTP request: \r\n%s\r\n", httpRequest);

		len = strlen(httpRequest);

		/* Send the message to the server */
		if (wolfSSL_write(ssl, httpRequest, strlen(httpRequest)) != len)
		{
			HTTPS_DEBUG_PRINTF("wolfSSL_write fail!\r\n");
			errorCode = MLS_ERROR;
			break;
		}

		HTTPS_DEBUG_PRINTF("wolfSSL_write success!\r\n");

		/* Read the server data into our buff array */
		memset(httpsResponse, 0, responseSize);

		while(True)
		{
			len = wolfSSL_read(ssl, pData + index, responseSize-1-index);
			if(len > 0)
			{
				// Look up "\r\n\r\n" Detect the start of content
				Bool ret1 = myStrStr("SUCCESS", pData);
				Bool ret2 = myStrStr("Duplicate Transaction", pData);

				if (ret1 || ret2)
				{
					break;
				}

				HTTPS_DEBUG_PRINTF("Temp break. Not found \"SUCCESS\" or \"Duplicate Transaction\"\r\n");
				break;
			}
			else if(len < 0)
			{
				errorCode = MLS_ERROR;
				break;
			}
		}

		/* Print to stdout any data the server sends */
		HTTPS_DEBUG_PRINTF("HTTPS response: %s\n", httpsResponse);

	}while(0);

	if (sockfd > 0)
	{
		/* Cleanup and return */
		if (ssl)
		{
			wolfSSL_free(ssl);      		/* Free the wolfSSL object                  */
			HTTPS_DEBUG_PRINTF("\r\nSSL free\r\n");
		}

		if (ctx)
		{
			wolfSSL_CTX_free(ctx);  		/* Free the wolfSSL context object          */
			HTTPS_DEBUG_PRINTF("SSL_CTX free\r\n");
		}

		wolfSSL_Cleanup();      		/* Cleanup the wolfSSL environment          */
		HTTPS_DEBUG_PRINTF("SSL_Cleanup success\r\n");

		mlsOsalMutexUnlock(&gSslMutex);

		mlsNetworkCloseSocket(sockfd);	/* Close the connection to the server       */
	}
	else
	{
		mlsOsalMutexUnlock(&gSslMutex);
	}

	gUpdateRssi = True;
	/******************************SSL/TLS End**************************************/

	return errorCode;
}

/**@}*/
