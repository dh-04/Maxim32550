/**********************************************************************************
 *  (C) Copyright 2009 Molisys Solutions Co., Ltd. , All rights reserved          *
 *                                                                                *
 *  This source code and any compilation or derivative thereof is the sole        *
 *  property of Molisys Solutions Co., Ltd. and is provided pursuant to a         *
 *  Software License Agreement.  This code is the proprietary information         *
 *  of Molisys Solutions Co., Ltd and is confidential in nature.  Its use and     *
 *  dissemination by any party other than Molisys Solutions Co., Ltd is           *
 *  strictly limited by the confidential information provisions of the            *
 *  Agreement referenced above.                                                   *
 **********************************************************************************/

/**
 * @addtogroup mlsInclude
 * @{
 */

/**
 * @file mlsMacros.h
 * @brief	Common macro definitions
 *
 * This file contains common macro which are used a lot of time in project
 * @author	mlsvnusr001
 */

#ifndef __MLMACRO_H__
#define __MLMACRO_H__

#include "../include/mlsTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define MAX_UINT32 (0xFFFFFFFF)

#define APDU_CLA(buff) (buff[0])
#define APDU_INS(buff) (buff[1])
#define APDU_P1(buff) (buff[2])
#define APDU_P2(buff) (buff[3])
#define APDU_P3_LX(buff) (buff[4])
#define APDU_DATA(buff) (buff[5])

#define MLS_MACRO_CHAR_TO_INT32(a,b,c,d)     \
    ((UInt32) ((UInt8) (a) << 24 | (UInt8) (b) << 16 | (UInt8) (c) << 8 | (UInt8) (d)))

#define MLS_MACRO_MAX( __x, __y )  ((__x) > (__y) ? (__x) : (__y))

#define MLS_MACRO_MIN( __x, __y ) ((__x) < (__y) ? (__x) : (__y))

#define MLS_MACRO_ABS( __x ) (((__x) > 0) ? (__x) : -(__x))

#define MLS_MACRO_IS_DIGIT(_c_)	( (_c_ >= '0') && (_c_ <= '9') )

	/* The value __x or the bound __y or __z if outside them */
#define MLS_MACRO_LIMIT( __x, __y, __z ) MAX( MIN( (__x), MAX( (__y), (__z) )), MIN( (__y), (__z) ))

#define MLS_ASSERT(__x, __y) {while(__x == __y);}

#define MLS_ASSERT_IFNOT(__x, __y) {while(__x != __y);}


#define MLS_UNUSED_PARAMETER(param) ((void)(param))

#define MLS_ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

#ifndef ABS
#define ABS MLS_MACRO_ABS
#endif

#ifndef LIMIT
#define LIMIT MLS_MACRO_LIMIT
#endif

#ifndef ASSERT
#define ASSERT(a,b)
#endif

#ifdef assert
#undef assert
#endif
#define assert(e) if(!(e))while(1)

#define UNUSED_PARAM(param)	((void)(param))

#define INFINITE            0xFFFFFFFF  // Infinite timeout
#define SWAP_16(x) ((((x) >> 8)&0xFF) | (((x) << 8)&0xFF00))
#define SWAP_32(x) ((((x) >> 24)&0xFF) | (((x) >> 8) & 0x0000FF00)\
                        |(((x) << 8) & 0x00FF0000) | (((x) << 24)& 0xFF000000))

#define MLS_BYTE_0(x) ((UInt8)(x))
#define MLS_BYTE_1(x) ((UInt8)(x>>8))
#define MLS_BYTE_2(x) ((UInt8)(x>>16))
#define MLS_BYTE_3(x) ((UInt8)(x>>24))

#define BE3BytesToInt32(buff)	((Int32)(((((Int8*)(buff))[0] << 16) & 0x00FF0000) | 			\
								((((Int8*)(buff))[1] << 8) & 0x0000FF00) | 				\
								(((Int8*)(buff))[2] & 0x000000FF)|						\
								((((Int8*)(buff))[0] < 0) ? 0xFF000000 : 0x00000000)))
#define Int32ToBE3Bytes(num, buff) 		\
	do{									\
		(buff)[0] = ((num) >> 16) & 0xFF; \
		(buff)[1] = ((num) >> 8) & 0xFF; 	\
		(buff)[2] = (num) & 0xFF; 		\
	}while(0)

#define	TO_BCD_8(x)		(((x) % 10) | ((((x) / 10) %10) << 4))
#define	TO_BCD_16(x)	(((x) % 10) | ((((x) / 10) %10) << 4) 								\
						| ((((x) / 100) %10) << 8) | ((((x) / 1000) %10) << 12))
#define	TO_BCD_32(x)	(((x) % 10) | ((((x) / 10) %10) << 4) 								\
						| ((((x) / 100) %10) << 8) | ((((x) / 1000) %10) << 12)				\
						| ((((x) / 10000) %10) << 16) | ((((x) / 100000) %10) << 20)		\
						| ((((x) / 1000000) %10) << 24) | ((((x) / 10000000) %10) << 28))

#define	CHECK_ERROR(func)				\
	do{									\
		errCode = func;					\
		if (errCode != MLS_SUCCESS){	\
			goto cleanup;				\
		}								\
	}while(0)

/*! \def CHECK_SUCCESS(status)

 */
#ifndef CHECK_SUCCESS
#define CHECK_SUCCESS(status)         	{if ((status) != 0) {return (status);}}
#endif
/*! \def CHECK_SUCCESS_FCT(status,fct)

 */
#ifndef CHECK_SUCCESS_FCT
#define CHECK_SUCCESS_FCT(status,fct) 	{(status) = (fct); CHECK_SUCCESS(status);}
#endif

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

#ifdef __cplusplus
}
#endif

#endif /*__MLMACRO_H__  */

/*@}*/
