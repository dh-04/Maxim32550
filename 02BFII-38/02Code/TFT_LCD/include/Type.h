
#ifndef __TWO_H__
#define __TWO_H__
/* C++ detection */
#ifdef __cplusplus
extern "C"
{
#endif

typedef signed char            Int8;
typedef unsigned char          UInt8;
typedef signed short           Int16;
typedef unsigned short         UInt16;
typedef signed int             Int32;
typedef unsigned int           UInt32;
typedef signed long            IntPtr;
typedef unsigned long          UIntPtr;
typedef int 				   mlsErrorCode_t;

typedef enum {
	MLS_SUCCESS = 0,
} ErrorCode_t;

#ifdef __cplusplus
}
#endif
#endif /* SOURCE_STYL_MLSLCD_INC_MLSLCD_H_ */
