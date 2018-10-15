/**
 * @defgroup mlsRtc
 * @brief
 *
 * @{
 */

/**
 * @file mlsRtc.h
 * @brief mlsRtc component
 *
 * @date 	$ March, 2016
 * @author	$
 */
#ifndef SOURCE_STYL_MLSAPP_INC_MLSRTC_H_
#define SOURCE_STYL_MLSAPP_INC_MLSRTC_H_

#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ***************************************************/
#include "mlsInclude.h"
#include "../src/mlsOsal/inc/mlsOsal.h"
//#include "../mlsDateTime/mlsDateTime.h"
/********** Constant  and compile switch definition section *******************/

/********** Type definition section *******************************************/
typedef enum
{
	PRIOR_HIGH,
	PRIOR_NORMAL,
	PRIOR_LOW,
}mlsPriority_t;

typedef enum
{
	TIME_UTC,
	TIME_LOCAL
}mlsTimeType_t;

extern mlsMutexHandle_t gMutexRtc;
/********** Macro definition section*******************************************/

/********** Function declaration section **************************************/

/**
 * @brief Get the current RTC counter value
 *
 * @param valueRtc is the pointer to get the RTC counter
 *
 * @Return
 * @retval	MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: otherwise.
 */
mlsErrorCode_t mlsRtcGet(UInt32 *valueRtc, mlsTimeType_t type);

/**
 * @brief Set a value to RTC
 *
 * @param valueRtc is the value (number of seconds) to set to RTC counter
 *
 * @Return
 * @retval	MLS_SUCCESS: operation successful.
 * @retval  MLS_ERROR: otherwise.
 */
mlsErrorCode_t mlsRtcSet(UInt32 valueRtc, mlsPriority_t prior);

#ifdef __cplusplus
}
#endif


#endif /* SOURCE_STYL_MLSAPP_INC_MLSRTC_H_ */
