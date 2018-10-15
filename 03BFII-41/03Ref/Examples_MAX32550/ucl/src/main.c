/*
 * @file ucl/src/main.c
 *
 ******************************************************************************
 * Copyright (C) 2016-2018 Maxim Integrated Products, Inc., All rights Reserved.
 *
 * This software is protected by copyright laws of the United States and
 * of foreign countries. This material may also be protected by patent laws
 * and technology transfer regulations of the United States and of foreign
 * countries. This software is furnished under a license agreement and/or a
 * nondisclosure agreement and may only be used or reproduced in accordance
 * with the terms of those agreements. Dissemination of this information to
 * any party or parties not specified in the license agreement and/or
 * nondisclosure agreement is expressly prohibited.
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 ******************************************************************************
 */

/**
 * @defgroup EX_UCL UCL Example
 * @brief This example shows various Universal Crypto Library tests.
 * @{
 *
 *
 * @ingroup MML_Examples
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

/* Global includes */
#include <ucl_testing_config.h>

#ifndef SYSTEM_MINGW
#include <errors.h>
#include <string.h>
#endif//MINGW

#ifdef SYSTEM_CMSIS
#include <MAX325xx.h>
#endif /* SYSTEM_CMSIS */

/* Other includes */
#if  !defined(JIBE_LINUX)&& !defined(SYSTEM_MINGW) && !defined(SYSTEM_CMSIS)
#include <config.h>
#include <cobra_defines.h>
#include <cobra_macros.h>
#include <cobra_functions.h>
#include <uart_config.h>
#else
#if !defined(SYSTEM_MINGW) && !defined(SYSTEM_CMSIS)
#include <config.h>
#include "sys/time.h"
#include "perfs_test.h"
#endif//mingw
#endif//jibe_linux && mingw

#ifdef SYSTEM_LIGHTHOUSE
#include <config.h>
#include <mml_gcr.h>
#include <mml_gcr_regs.h>
#include <mml_uart.h>
#endif

#include <ucl/ucl_config.h>
#include <ucl/ucl_defs.h>
#include <ucl/ucl_retdefs.h>
#include <ucl/ucl_types.h>
#include <ucl/ucl_sys.h>
#include <ucl/ucl_stack.h>
#include <ucl/ucl_sha1.h>
#include <ucl/ucl_sha224.h>
#include <ucl/ucl_sha256.h>
#include <ucl/ucl_sha384.h>
#include <ucl/ucl_sha512.h>
#include <ucl/ucl_sm3.h>
#include <ucl/ucl_hmac_sha256.h>
#include <ucl/ucl_info.h>
#include <ucl/ucl_stest.h>

#include <ucl/ucl_hash.h>

/* Local includes */

#ifdef SYSTEM_LIGHTHOUSE
#include <config.h>
#include <printf_lite.h>
#endif

#if !defined(SYSTEM_MINGW) && !defined(SYSTEM_CMSIS)
#include <private.h>
#endif//mingw
#include <ucl_aes.h>
#include <pkcs5test.h>
#include <shatest.h>
#include <dsatest.h>
#include <dhtest.h>
#include <ecdsatest.h>
#include <rsatest.h>
#include <destest.h>
#include <3destest.h>
#include <aestest.h>
#include <trngtest.h>
#include <sp80056test.h>
#include <rsapkcs1test.h>
#include <rsax931test.h>
#include <aesgcmtest.h>

#define	K_UCL_USECASE_BUFFER_SIZE_INT		8192


#define EX_UCL_VERS_MAJOR	<VERSMAJ>
#define EX_UCL_VERS_MINOR	<VERSMIN>
#define EX_UCL_VERS_PATCH	<VERSPAT>
#define EX_UCL_VERSION_STRING	"v" xstr(EX_UCL_VERS_MAJOR) "." xstr(EX_UCL_VERS_MINOR) "." xstr(EX_UCL_VERS_PATCH)


u32 init_buffer[K_UCL_USECASE_BUFFER_SIZE_INT];

#include <ucl/ucl_hash.h>

#ifdef DSA
int test_dsa(void)
{
  int resu;
  PRINTF("DSA ");
  resu=dsatest_verify();
  if(UCL_OK!=resu)
    {
      PRINTF("NOK\n");
      return(UCL_ERROR);
    }
  else
    PRINTF(" OK\n");
  return(UCL_OK);
}
#endif

#ifdef HASH
void test_hash(void)
{
  PRINTF("HASH testing----------\n");
#ifdef HASH_SHA1
  if(UCL_OK==test_sha1())
    PRINTF("SHA1 OK\n");
  else
    PRINTF("SHA1 NOK\n");
#endif

#ifdef HASH_SM3
  if(UCL_OK==test_sm3())
    PRINTF("SM3 OK\n");
  else
    PRINTF("SM3 NOK\n");
#endif

#ifdef HASH_SHA256
  if(UCL_OK==test_sha256())
    PRINTF("SHA256 OK\n");
  else
    PRINTF("SHA256 NOK\n");
  if(UCL_OK==sample_hmac256())
    PRINTF("HMAC-256 OK\n");
  else
    PRINTF("HMAC-256 NOK\n");
#endif

#ifdef HASH_SIA256
  if(UCL_OK==test_sia256())
    PRINTF("SIA256 OK\n");
  else
    PRINTF("SIA256 NOK\n");
#endif

#ifdef HASH_SHA384
  if(UCL_OK==test_sha384())
    PRINTF("SHA384 OK\n");
  else
    PRINTF("SHA384 NOK\n");
  if(UCL_OK==sample_hmac384())
    PRINTF("HMAC-384 OK\n");
  else
    PRINTF("HMAC-384 NOK\n");
#endif

#ifdef HASH_SHA224
  if(UCL_OK==test_sha224())
    PRINTF("SHA224 OK\n");
  else
    PRINTF("SHA224 NOK\n");
  if(UCL_OK==sample_hmac224())
    PRINTF("HMAC-224 OK\n");
  else
    PRINTF("HMAC-224 NOK\n");
#endif

#ifdef HASH_SHA512
  if(UCL_OK==test_sha512())
    PRINTF("SHA512 OK\n");
  else
    PRINTF("SHA512 NOK\n");
  if(UCL_OK==sample_hmac512())
    PRINTF("HMAC-512 OK\n");
  else
    PRINTF("HMAC-512 NOK\n");
#endif
  PRINTF("\n");
}
#endif

#ifdef ECDSA
/*#undef P160
  #undef P192
  #undef P384
  #undef P521
  #undef SM2P256
  #undef BP256
  #undef BP384
  #undef BP512*/

void test_ecdsa(void)
{
  PRINTF("ECDSA testing----------\n");

#ifdef NIST_CURVES
  if(UCL_OK==test_ecdsa_curves())
    PRINTF("OK\n");
  else
    PRINTF("NOK\n");
  PRINTF("\n");
#endif /* NIST_CURVES */

#ifdef SM2P256
  PRINTF("SM2P256 ");
  if(UCL_OK==test_ecdsa_p256sm2())
    PRINTF("OK\n");
  else
    PRINTF("NOK\n");
    #endif /* SM2P256 */
}
#endif /*ECDSA */

#ifdef HASH
void test_hash_selftests(void)
{
int err;
PRINTF("TEST selftests SHA & HMAC-SHA ");
err=ucl_hash_sha224_stest();
if(UCL_OK!=err)
  PRINTF("ERROR self-test sha224\n");
 else
   {
err=ucl_hash_sha384_stest();
if(UCL_OK!=err)
  PRINTF("ERROR self-test sha384\n");
 else
   {
err=ucl_hash_sha512_stest();
if(UCL_OK!=err)
  PRINTF("ERROR self-test sha512\n");
 else
   {
err=ucl_hmac_sha1_stest();
if(UCL_OK!=err)
  PRINTF("ERROR self-test hmac sha1\n");
 else
   {
err=ucl_hmac_sha256_stest();
if(UCL_OK!=err)
  PRINTF("ERROR self-test hmac sha256\n");
 else
   {
err=ucl_hmac_sha224_stest();
if(UCL_OK!=err)
  PRINTF("ERROR self-test hmac sha224\n");
 else
   {
err=ucl_hmac_sha384_stest();
if(UCL_OK!=err)
  PRINTF("ERROR self-test hmac sha384\n");
 else
   {
err=ucl_hmac_sha512_stest();
if(UCL_OK!=err)
  PRINTF("ERROR self-test hmac sha512\n");
 else
   PRINTF("OK\n");
   }
   }
   }
   }
   }
   }
   }
}
#endif /* HASH */

#ifdef DSA
void test_dsa_selftest(void)
{
int resu;
PRINTF("TEST selftest DSA ");
resu=ucl_pkc_dsa_stest();
if(UCL_OK!=resu)
  PRINTF(" NOK\n");
 else
   PRINTF(" OK\n");
}
#endif /* DSA */

#ifdef ECDSA
void test_ecdsa_selftests(void)
{
  int resu;
  PRINTF("TEST selftests ECDSA ");

#ifdef VERBOSE
  PRINTF("224-224 ");
#endif /* VERBOSE */

  resu=ucl_ecdsa_p224r1_sha224_stest();
  if(UCL_OK!=resu)
    PRINTF("NOK\n");

#ifdef VERBOSE
  else
    PRINTF("OK\n");
#endif /* VERBOSE */

#ifdef P521

    #ifdef VERBOSE
  PRINTF("521-512 ");
#endif /* VERBOSE */

  resu=ucl_ecdsa_p521r1_sha512_stest();
  if(UCL_OK!=resu)
    PRINTF("NOK\n");

#ifdef VERBOSE
  else
    PRINTF("OK\n");
#endif /* VERBOSE */

#endif /* P521 */

#ifdef P384

#ifdef VERBOSE
  PRINTF("384-384 ");
#endif /* VERBOSE */

  resu=ucl_ecdsa_p384r1_sha384_stest();
  if(UCL_OK!=resu)
    PRINTF("NOK\n");

#ifdef VERBOSE
  else
    PRINTF("OK\n");
#endif /* VERBOSE */

#endif /* P384 */

#ifdef VERBOSE
  PRINTF("192-1 ");
#endif /* VERBOSE */

  resu=ucl_ecdsa_p192r1_sha1_stest();
  if(UCL_OK!=resu)
    PRINTF("NOK\n");

#ifdef VERBOSE
  else
    PRINTF("OK\n");
#endif
#ifdef VERBOSE
  PRINTF("192-256 ");
#endif /* VERBOSE */

  resu=ucl_ecdsa_p192r1_sha256_stest();
  if(UCL_OK!=resu)
    PRINTF("NOK\n");

#ifdef VERBOSE
  else
    PRINTF("OK\n");
#endif /* VERBOSE */

#ifdef VERBOSE
  PRINTF("256-1 ");
#endif /* VERBOSE */

  resu=ucl_ecdsa_p256r1_sha1_stest();
  if(UCL_OK!=resu)
    PRINTF("NOK\n");

#ifdef VERBOSE
  else
    PRINTF("OK\n");
#endif /* VERBOSE */

  PRINTF("256-256 ");
  resu=ucl_ecdsa_p256r1_sha256_stest();
  if(UCL_OK!=resu)
    PRINTF("NOK\n");

#ifdef VERBOSE
  else
    PRINTF("OK\n");
#endif /* VERBOSE */

#ifdef VERBOSE
  PRINTF("160-1 ");
#endif

  resu=ucl_ecdsa_p160r1_sha1_stest();
  if(UCL_OK!=resu)
    PRINTF("NOK\n");

#ifdef VERBOSE
  else
    PRINTF("OK\n");
#endif /* VERBOSE */

  PRINTF("160-256 ");
  resu=ucl_ecdsa_p160r1_sha256_stest();
  if(UCL_OK!=resu)
    PRINTF("NOK\n");

#ifdef VERBOSE
  else
    PRINTF("OK\n");
#endif /* VERBOSE */

  PRINTF("OK\n");
}
#endif//ECDSA

int ucl_testing(void)
{
int result=UCL_OK;
int err;

/* Welcome message */
PRINTF("Welcome to Usecase UCL Application RSA/ECDSA/SHA/AES/3DES\n");
#ifndef SYSTEM_MINGW

mml_rtc_reset();

mml_rtc_start_up();

#endif /* SYSTEM_MINGW */

test_trng();
err = ucl_init(init_buffer,sizeof(init_buffer)/sizeof(init_buffer[0]));
if(err!=UCL_OK)
  {
PRINTF("ERROR for ucl_init %d\n",err);
return(err);
}
 else
   PRINTF("ucl_init ok with %d bytes\n",sizeof(init_buffer));

#ifdef DES
PRINTF("DES ");
#endif /* DES */

#ifdef TDES
PRINTF("3DES ");
#endif /* TDES */

#ifdef AES
PRINTF("AES ");
#endif /* AES */

#ifdef RSA
PRINTF("RSA ");
#endif /* RSA */

#ifdef X931
PRINTF("RSA-X931 ");
#endif /* X931 */

#ifdef PKCS1
PRINTF("RSA-PKCS1 ");
#endif /* PKCS1 */

#ifdef SP80056
PRINTF("SP800-56 ");
#endif /* SP80056 */

#ifdef DSA
PRINTF("DSA ");
#endif /* DSA */

#ifdef ECDSA
PRINTF("ECDSA ");
#ifdef SM2P256
PRINTF("SM2-P256 ");
#endif /* SM2P256 */

#ifdef BP256
PRINTF("BP256 ");
#endif /* BP256 */

#ifdef BP384
PRINTF("BP384 ");
#endif /* BP384 */

#ifdef BP512
PRINTF("BP512 ");
#endif /* BP512 */

#ifdef P256
PRINTF("P256 ");
#endif /* P256 */

#ifdef P224
PRINTF("P224 ");
#endif /* P224 */

#ifdef P521
PRINTF("P521 ");
#endif /* P521 */

#ifdef P192
PRINTF("P192 ");
#endif /* P192 */

#ifdef P384
PRINTF("P384 ");
#endif /* P384 */

#ifdef SIA256
PRINTF("SIA256 ");
#endif /* SIA256 */

#ifdef P160
PRINTF("P160");
#endif /* P160 */

#endif /* ECDSA */
PRINTF("\n");

#ifdef USE_TRNG
PRINTF("TRNG ");
if(UCL_OK!=sample_trng(1))
  PRINTF("NOK\n");
 else
   if (UCL_OK!=test_trng())
     PRINTF("Error on test_trng\n");
   else
     if (UCL_OK!=test_whitening_trng())
       PRINTF("Error on whitening TRNG\n");
     else
       {
	 if (UCL_OK!=test_emv_unpredictable_number())
	   PRINTF("Error on EMV UN %d\n", result);
	 else
	   PRINTF("OK\n");
       }
#endif//USE_TRNG

#ifdef PKCS5
PRINTF("PKCS5 ");
if(UCL_OK!=sample_pkcs5())
  PRINTF("NOK\n");
 else
   PRINTF("OK\n");
#endif /* PKCS5 */

#ifdef TDES
if(UCL_OK==sample_3des(1))
  PRINTF("3DES OK\n");
 else
   PRINTF("3DES NOK\n");
#endif /* TDES */

#ifdef AES
if(UCL_OK==sample_aes())
  PRINTF("AES OK\n");
 else
   PRINTF("AES NOK\n");

#ifdef VERBOSE
if(UCL_OK==stress_aes(5000))
  PRINTF("AES OK\n");
 else
   PRINTF("AES NOK\n");
#endif/* VERBOSE */

#ifdef AES_GCM
test_gcm();
#endif /* AES_GCM */

#endif /* AES */

#ifdef HASH
test_hash();
#endif /* HASH */

#ifdef ECDSA
test_ecdsa();
#endif /* ECDSA */

#ifdef SP80056
PRINTF("SP800-56 ");
if(UCL_OK==test_sp800_56(4))
  PRINTF("OK\n");
 else
   PRINTF("NOK\n");
#endif /* SP80056 */

#ifdef DSA
test_dsa();
#endif /* DSA */

#ifdef RSA
test_rsa(1);
test_rsa_crt_parameters(1);
#endif /* RSA */

#ifdef PKCS1
test_pkcs1(1);
#endif /* PKCS1 */

#ifdef X931
test_x931(1);
#endif /* X931 */

#ifdef DH
PRINTF("DH ");
if(UCL_OK==sample_dh())
  PRINTF("OK\n");
 else
   PRINTF("NOK\n");
#endif /* DH */

#ifdef HASH
test_hash_selftests();
#endif /* HASH */

#ifdef ECDSA
test_ecdsa_selftests();
#endif /* ECDSA */

#ifdef DSA
test_dsa_selftest();
#endif /* DSA */

return result;
}

int main(void)
{
  int result=UCL_OK;
#if !defined(JIBE_LINUX) && !defined(SYSTEM_MINGW)
  mml_uart_config_t		uart_conf;

  // Initialize UATR0 port with default configurations
  uart_conf.baudrate = 115200;
  uart_conf.data_bits = UARTn_CTRL_SIZE_bits8;
  uart_conf.flwctrl = UARTn_CTRL_RTSCTSF_disable;
  uart_conf.parity = MML_UART_PARITY_NONE;
  uart_conf.parity_mode = MML_UART_PARITY_MODE_ONES;
  uart_conf.rts_ctl = UARTn_PIN_RTS_lo;
  uart_conf.stop_bits = UARTn_CTRL_STOP_stop1;
  result = mml_uart_init(MML_UART_DEV0, uart_conf);
  if ( result )
  {
      return result;
  }
#endif//jibe-linux && mingw
  PRINTF("UCL validation tool %s\n", EX_UCL_VERSION_STRING);
  ucl_testing();

#ifdef JIBE_LINUX
  perftest(1000, 50<<10); //1000 loops of 50kbytes each
#endif
  PRINTF("this is the end...\n");
  // You may want to hold application execution
#ifndef SYSTEM_MINGW
  while( 1 );
#endif
  return result;
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__

#ifdef __RAM_FWK__
#error "This example does not handle RAM Target"
#endif //__RAM_FWK__

/******************************************************************************/
// EOF
