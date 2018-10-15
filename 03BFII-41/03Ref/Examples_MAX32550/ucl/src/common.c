/*
 * common.c --
 *
 * ----------------------------------------------------------------------------
 * Copyright (c) 2014-2016, Maxim Integrated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Maxim Integrated nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MAXIM INTEGRATED ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//Global includes
#include <ucl_testing_config.h>

#ifdef SYSTEM_CMSIS
#include <MAX325xx.h>
#endif /* SYSTEM_CMSIS */

#if defined(__max32550)||defined(__max32555)||defined(__lighthouse)||defined(__yumen)
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
#include <ucl/ucl_sha256.h>
#include <ucl/ucl_hmac_sha256.h>
#include <ucl/ucl_info.h>
#include <ucl/ucl_stest.h>
// Local includes

#ifdef __max32550
#include <printf_lite.h>
#endif

#ifdef __max32555
#include <printf_lite.h>
#endif

#ifdef __max32620
#include <max32620.h>
#include <rtc_regs.h>
#include <rtc.h>
#include <io.h>
#endif

#ifdef __max32621
#include <max32621.h>
#include <rtc_regs.h>
#include <rtc.h>
#include <io.h>
#endif

#ifdef __max32631
#include <max3263x.h>
#include <rtc_regs.h>
#include <rtc.h>
#include <io.h>
#endif

#ifdef __max32630
#include <max3263x.h>
#include <rtc_regs.h>
#include <rtc.h>
#include <io.h>
#endif

u32 general_time;
u32 prev_general_time;

void display(char *string,u8 *tab,int size)
{
  int i;
  PRINTF("%s",string);
  for(i=0;i<size;i++)
    PRINTF("%02x",tab[i]);
  PRINTF("\n");
}

#if defined( __max32620)||defined( __max32621)||defined( __max32630)||defined( __max32631)
int getchipversion(void)
{
  /*typedef volatile unsigned int io_register_t;

#define IO_READ_U32( _register_, _value_ ) \
((_value_) = *((io_register_t *)(_register_)))*/

#define FLASH_INFOADDR_A0 0x400010A0
  u32 value;
  IO_READ_U32(FLASH_INFOADDR_A0,value);
  return(value);
}

void rtc_reset(void)
{
  RTC_Start();
}

void start_rtc(char debug)
{
  RTC_Start();
}

void give_seconds(u32 *seconds)
{
  u32 value;
  value=RTC_GetCount();
  *seconds=value;
}

#else
#if  !defined(JIBE_LINUX)&& !defined(SYSTEM_MINGW)

#ifndef SYSTEM_CMSIS
#define RTC_BASE 		0x40006000
#define RTC_RTCSEC		RTC_BASE + 0x0
#define RTC_RTCSSEC		RTC_BASE + 0x04
#define RTC_RTCCN		RTC_BASE + 0x10

void rtc_reset(void)
{
	unsigned int value;

	// poll on RTC busy bit
	IO_READ_U32(RTC_RTCCN, value);
	while( ((value>>3) & 0x1) == 1)
		IO_READ_U32(RTC_RTCCN, value);
	// set RTC ctrl write enable
	IO_WRITE_U32(RTC_RTCCN, 0x8000);

	// poll on RTC busy bit
	IO_READ_U32(RTC_RTCCN, value);
	while( ((value>>3) & 0x1) == 1)
		IO_READ_U32(RTC_RTCCN, value);
	// reset RTC second register
	IO_WRITE_U32(RTC_RTCSEC, 0x00000000);

	// poll on RTC busy bit
	IO_READ_U32(RTC_RTCCN, value);
	while( ((value>>3) & 0x1) == 1)
		IO_READ_U32(RTC_RTCCN, value);
	// set RTC control register (quiet mode)
	IO_WRITE_U32(RTC_RTCCN, 0x8000);

}

/* start RTC */
void start_rtc(char debug)
{
	unsigned int value;


	/* Wait for the busy flag */
	IO_READ_U32(RTC_RTCCN, value);
	while( ((value>>3) & 0x1) == 1)
		IO_READ_U32(RTC_RTCCN, value);

	if(debug == 1)
	{
		//read RTC control register
		IO_READ_U32(RTC_RTCCN, value);
		PRINTF("RTC control register: 0x%08X\n\n", value);
	}

	/* Wait for the busy flag */
	IO_READ_U32(RTC_RTCCN, value);
	while( ((value>>3) & 0x1) == 1)
		IO_READ_U32(RTC_RTCCN, value);
	// define second value
	//printf("Init RTC second\n");
	IO_WRITE_U32(RTC_RTCSEC, 0x00000000);

	/* Wait for the busy flag */
	IO_READ_U32(RTC_RTCCN, value);
	while( ((value>>3) & 0x1) == 1)
		IO_READ_U32(RTC_RTCCN, value);
	//printf("Init RTC sub-second\n");
	// define second value
	IO_WRITE_U32(RTC_RTCSSEC, 0x00000000);

	// poll on RTC busy bit
	IO_READ_U32(RTC_RTCCN, value);
	while( ((value>>3) & 0x1) == 1)
		IO_READ_U32(RTC_RTCCN, value);
	//printf("RTC write enable...\n");
	// set RTC ctrl write enable
	IO_WRITE_U32(RTC_RTCCN, 0x8000);

	// poll on RTC busy bit
	IO_READ_U32(RTC_RTCCN, value);
	while( ((value>>3) & 0x1) == 1)
		IO_READ_U32(RTC_RTCCN, value);
	// set RTC control register (quiet mode)
	IO_WRITE_U32(RTC_RTCCN, 0xC801);


	if(debug == 1)
	{
		/* Wait for the busy flag */
		IO_READ_U32(RTC_RTCCN, value);
		while( ((value>>3) & 0x1) == 1)
			IO_READ_U32(RTC_RTCCN, value);
		//read back RTC control register
		IO_READ_U32(RTC_RTCCN, value);
		PRINTF("RTC control register: 0x%08X\n\n", value);

		/* Wait for the busy flag */
		IO_READ_U32(RTC_RTCCN, value);
		while( ((value>>3) & 0x1) == 1)
			IO_READ_U32(RTC_RTCCN, value);
		IO_READ_U32(RTC_RTCSEC, value);
		PRINTF("RTC second register: 0x%08X\n\n", value);
	}
}

void give_seconds(u32 *seconds)
{
  u32 tmp;
  IO_READ_U32(RTC_BASE, tmp);
  *seconds=tmp;
}
#else

/* RTC reset */
void rtc_reset(void){

	mml_rtc_disable();

	/* Set to 0 second register */
	while ((RTC->CN & RTC_CN_BUSY_Msk) == 1);
	RTC->SEC = 0;

	/* Set to 0 sub second register */
	while ((RTC->CN & RTC_CN_BUSY_Msk) == 1);
	RTC->SSEC;

	mml_rtc_enable();

	/* Disable RTCE write */
	RTC->CN &= ~(RTC_CN_WE_Msk);

}

/* start RTC */
void start_rtc(void){

	/* Enable RTCE write */
	RTC->CN |= RTC_CN_WE_Msk;

	/* Disable RTCE */
	while ((RTC->CN & RTC_CN_BUSY_Msk) == 1);
	RTC->CN &= ~(RTC_CN_RTCE_Msk);

	/* Init second value */
	while ((RTC->CN & RTC_CN_BUSY_Msk) == 1);
	RTC->SEC = 0;

	/* Set sub second value */
	while ((RTC->CN & RTC_CN_BUSY_Msk) == 1);
	RTC->SSEC = 0;

	/* Enable RTC after reset */
	while ((RTC->CN & RTC_CN_BUSY_Msk) == 1);
	RTC->CN |= (RTC_CN_RTCE_Msk);

	/* Disable RTCE write */
	RTC->CN &= ~(RTC_CN_WE_Msk);
}

/* Read RTC second register */
void give_seconds(u32 *seconds){
	while ((RTC->CN & RTC_CN_RDY_Msk) == 0);
	*seconds = RTC->SEC;
}
#endif /* SYSTEM_CMSIS */

#else
#ifndef SYSTEM_MINGW
static struct timeval _TimeOrigin;

void start_rtc(char debug)
{
    (void) debug;
    gettimeofday(&_TimeOrigin, NULL);
}

void give_seconds(u32 *mseconds)
{
   static struct timeval _t;
   gettimeofday(&_t, NULL);
   *mseconds =  (u32) ((_t.tv_sec-_TimeOrigin.tv_sec)*1000) + (u32) ((_t.tv_usec - _TimeOrigin.tv_usec)/1000);
   _TimeOrigin = _t;
}
#else//mingw
#include <time.h>
void give_seconds(u32 *mseconds)
{
  *mseconds=(u32)time(NULL);
}
#endif//mingw
#endif
#endif//


void display_word(char *string,u32 *tab,int size)
{
  int i;
  PRINTF("%s",string);
  for(i=0;i<size;i++)
    PRINTF("%08x",tab[i]);
  PRINTF("\n");
}

void display_time(void)
{
  u32 factor=1;
  u32 seconds;
  //displaying seconds
#if defined( __max32620)||defined( __max32621)||defined( __max32630)||defined( __max32631)
  factor=4000;
#endif
  give_seconds(&seconds);
  general_time=seconds;
#if defined( __max32620)||defined( __max32621)||defined( __max32630)||defined( __max32631)
  PRINTF("->%.1fs\n",(general_time-prev_general_time)/(1.0*factor));
#else
  PRINTF("->%ds\n",(general_time-prev_general_time)/(factor));
#endif
  prev_general_time=general_time;
  /** We're done */
  return;
}
