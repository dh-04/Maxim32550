//this file is used to define which algorithms will be tested
//this is a multi-platform file
//the platform is automatically managed by the makefile of the project

// ** DO NOT MODIFY THE FOLLOWING LINES
#include <ucl/ucl_types.h>

//#define VERBOSE
//#define SYSTEM_HDE
//#define SYSTEM_EMBEDDED_LINUX
//#define SYSTEM_DESKTOP_LINUX
//#define SYSTEM_MINGW
#ifndef _UCL_TESTING_CONFIG_H
#define _UCL_TESTING_CONFIG_H
#ifdef __x86
#define SYSTEM_MINGW
#endif
#ifdef __lighthouse
#define SYSTEM_LIGHTHOUSE
#endif
#ifdef __jibe
#define SYSTEM_JIBE
#endif

#ifdef SYSTEM_JIBE
#define PRINTF lite_printf
#endif
#ifdef SYSTEM_LIGHTHOUSE
#define PRINTF lite_printf
#endif

#ifdef SYSTEM_TRANGO
#define PRINTF PRINTF
#define start_clock() 
#define end_clock(a, b, c)
#define P192_SIZE 24
#define P192_S 6
#define P192_BSIZE 192
#endif

#ifdef SYSTEM_HDE
#define PRINTF printf
#endif

#ifdef SYSTEM_EMBEDDED_LINUX
#define PRINTF printf
#endif

#ifdef SYSTEM_DESKTOP_LINUX
#define PRINTF printf
#endif

#ifdef SYSTEM_MINGW
#include <stdio.h>
#define PRINTF printf
#endif

#ifdef SYSTEM_CMSIS
#include <stdio.h>
#define PRINTF printf
#endif/* SYSTEM_CMSIS */

void give_seconds(u32 *seconds);

#define DES_STRESS_SIZE 2048
#define AES_STRESS_SIZE 2048

// NOW, YOU CAN MODIFY (comment/uncomment) THE FOLLOWING LINES
//#define TEA
//SYMMETRIC is for selecting symmetric algorithms (aes, 3des)
//#define SYMMETRIC
//HASH is for selecting hash functions and hmac
//#define HASH
//ASYMMETRIC is for selecting public-key crypto: rsa, ecdsa, dh, ..
//#define ASYMMETRIC
//FULLTEST is for performing stress computations and not only KAT
// it is useful for performances evaluation
//#define FULLTEST

#define USE_TRNG

#ifdef SYMMETRIC
#define TEA
#define DES
#define TDES
//UAES is only for USIP (ds5300)
//#define UAES
#define AES
#define AES_GCM
#define PKCS5
#endif
//HASH_LONG_TEST is for long hash KAT
//#define HASH_LONG_TEST
#ifdef ASYMMETRIC
#ifdef __usip
#define EC
#define B191
#define B163
#define P192
#define P256
#define ECDSAVS
#endif//usip

//#define ECDSA
#define DSA
#define RSA
#define PKCS1
#define X931
#define SP80056
#define DH
#endif

#ifdef PKCS1
#define PKCS1_LOOP 20
#endif

#ifdef ECDSA
#define NIST_CURVES
#define OLD_API
  #define MAX_ECDSA_CTEST 50
  #define MAX_ECDSA_VALID 5000
  #define MAX_ECDSA_PERFS 10
  #define MAX_MSG_LEN 100
#define P160
#define P256
#define P192
#define P384
#define P521
#define SM2P256
#define BP256
#define BP384
#define BP512
#undef P160
#undef P256
#undef P192
#undef P384
#undef P521
#define SM2P256
#undef BP256
#undef BP384
#undef BP512
#endif

int ucl_testing(void);
void display_time(void);
void display(char *string,u8 *tab,int size);
#endif//UCL_TESTING_CONFIG_H
