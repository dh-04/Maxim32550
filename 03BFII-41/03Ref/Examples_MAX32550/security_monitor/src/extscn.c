/*
 * @file security_monitor/src/extscn.c
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
 * trademarks, mask works rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 ******************************************************************************
 */

/* Global includes */
#include <MAX325xx.h>
#include <errors.h>
#include <stdio.h>
/* Local includes */
#include <extscn.h>
/* UCL Include */
#include <ucl/ucl_config.h>
#include <ucl/ucl_defs.h>
#include <ucl/ucl_retdefs.h>
#include <ucl/ucl_types.h>
#include <ucl/ucl_rng.h>
#include <ucl/ucl_sha256.h>
#include <ucl/ucl_sp80090a_hash_drbg.h>
#include <ucl/lighthouse_crypto.h>

extern unsigned int *ptr_NVSRAM;
int entropy_byte_size=1024;
u8 entropy_input[1024];
int nonce_byte_size=1024;
u8 nonce[1024];
ucl_sp80090a_internal_state_t internal_state;
u8 personalization_string[10];

void NMI_Handler(void);

__attribute__((section(".ram_code"))) void NMI_Handler(void) {
	printf("\n[-----------------------------------------------]\n");
	printf("[                  NMI - DRS                    ]\n");
	printf("[-----------------------------------------------]\n");
	/* Find attack source */
	printf("Security alarm registers: 0x%08x\n", (unsigned int)SMON->SECALM);
	/* Perform needed action according to secalm register */
	/* For example */
	check_aes_key();
	/* Acknowledge NMI IRQ */
	NVIC->ICPR[0] = (1 << 2);
	printf("/* Now reset platform */\n ");
	mml_gcr_dev_reset(MML_RSTR_DEV_ALL);
}

int sp80090_initialization(void)
{
	int resu;
	//the hash sp800-90a uses an internal state
	//the instantiation is used for initializing the internal state
	//before the generation of pseudorandom bits is possible.
	//it has to be done once.
	//For that, it needs a string of bits coming from the randomness source
	//the ucl_rng_read below is used to acquire this string of bits, stored in entropy_input
#ifdef VERBOSE
	printf("instantiate\n");
#endif
	resu=ucl_rng_read(entropy_input,entropy_byte_size,UCL_RAND_DEFAULT);
	if(entropy_byte_size!=resu)
		return(UCL_ERROR);
	//the nonce input can be a random string of bits (see SP800-90A section 8.6.7 for more options
	resu=ucl_rng_read(nonce,nonce_byte_size,UCL_RAND_DEFAULT);
	if(nonce_byte_size!=resu)
		return(UCL_ERROR);
	resu=ucl_sp80090a_instantiate_sha256(&internal_state,entropy_input,entropy_byte_size,nonce,nonce_byte_size,personalization_string,0);
	printf("init reseed_limit=%llx\n",internal_state.reseed_limit);
	return(resu);

}

int sp80090_read(u8 *pseudorandom_numbers,int pseudorandom_numbers_byte_size)
{
	int resu;
	//once the instantiation has been performed, the pseudo random numbers generation can start
	//generating numbers
	printf("generate\n");
	resu=ucl_sp80090a_hash_drbg_sha256_generate(pseudorandom_numbers,pseudorandom_numbers_byte_size*8,&internal_state, NULL, 0);
	if(UCL_RESEED_REQUIRED==resu)
	{
		printf("reseeding\n");
		//reseeding only, not generating any number
		resu=ucl_rng_read(entropy_input,entropy_byte_size,UCL_RAND_DEFAULT);
		if(resu!=entropy_byte_size)
			return(UCL_ERROR);
		resu=ucl_sp80090a_reseeding_sha256(&internal_state,entropy_input,entropy_byte_size,nonce,0);
		printf("reseed_counter=%llx\n",internal_state.reseed_counter);
		if(UCL_OK!=resu)
			return(resu);
		else
		{
			resu=ucl_sp80090a_hash_drbg_sha256_generate(pseudorandom_numbers,pseudorandom_numbers_byte_size*8,&internal_state, NULL, 0);
			printf("resu=%d\n",resu);
			return(resu);
		}
	}
	else
	{
		if(UCL_OK!=resu)
		{
#ifdef VERBOSE
			printf("error hash drbg: %d\n",resu);
#endif
			return(UCL_ERROR);
		}
	}
	return(UCL_OK);
}

int sp80090_usage(u8 *random)
{
	int resu;
	int total_numbers_byte_size;
	u8 pseudorandom_numbers[32];
	int pseudorandom_numbers_byte_size=31;
	int i=0;

#ifdef VERBOSE
	int j;
#endif

	total_numbers_byte_size=32;

	sp80090_initialization();
	while(i<total_numbers_byte_size)
	{
		resu=sp80090_read(pseudorandom_numbers,pseudorandom_numbers_byte_size);
		if(UCL_OK!=resu)
		{
			printf("resu=%d\n",resu);
			return(resu);
		}
#ifdef VERBOSE
		for(j=0;j<pseudorandom_numbers_byte_size;j++)
			printf("%02x",pseudorandom_numbers[j]);
		printf("\n");
#endif
		i+=pseudorandom_numbers_byte_size;
	}

	for(i=0; i<total_numbers_byte_size ; i++) {
		random[i] = pseudorandom_numbers[i];
	}

	return(UCL_OK);
}

#if 0
int sp80090_usage(u8 *random)
{
	u8 pseudorandom_numbers[32];
	int resu;
	int total_numbers_byte_size;
	int i=0;
	int pseudorandom_numbers_byte_size=32;

	total_numbers_byte_size=32;

	sp80090_initialization();
	while(i<total_numbers_byte_size)
	{
		resu=sp80090_read(pseudorandom_numbers, pseudorandom_numbers_byte_size);
		if(UCL_OK!=resu)
			return(resu);
		i+=pseudorandom_numbers_byte_size;
	}

	for(i=0; i<total_numbers_byte_size ; i++) {
		random[i] = pseudorandom_numbers[i];
	}

	return(UCL_OK);
}
#endif

void wfa(void) {
	while ((SMON->SECST & 0x7) != 0x0);
}

void get_aes_result(unsigned int *data) {
	/* Clear CPH_DONE */
	CRYPTO->CRYPTO_CTRL = 0;
	/* Compute AES encryption with user or secure key, 128-bit key length */
	/* Compute AES encryption with User key, 256-bit key length */
	CRYPTO->CIPHER_CTRL = 0x18;
	CRYPTO->CRYPTO_DIN_0 = data[0];
	CRYPTO->CRYPTO_DIN_1 = data[1];
	CRYPTO->CRYPTO_DIN_2 = data[2];
	CRYPTO->CRYPTO_DIN_3 = data[3];
	/* Poll to wait for AES completion */
	while (!(( CRYPTO->CRYPTO_CTRL >> 27) & 0x1));
	/* Clear CPH_DONE */
	CRYPTO->CRYPTO_CTRL = 0;
	/* Read computed data */
	data[0] = CRYPTO->CRYPTO_DOUT_0;
	data[1] = CRYPTO->CRYPTO_DOUT_1;
	data[2] = CRYPTO->CRYPTO_DOUT_2;
	data[3] = CRYPTO->CRYPTO_DOUT_3;
}

int set_sensors(char internal, unsigned int int_conf, char external,
		unsigned int ext_conf, unsigned char *status) {

	unsigned int i;
	for (i = 0; i < NUMBER_OF_EXT_SENS; i++) {
		status[i] = IDLE;
	}
#ifdef _UNLOCK_
	printf("********************************\n");
	printf("*   EXT SENSORS _UNLOCKED       *\n");
	printf("********************************\n");
#endif /* _UNLOCK_ */
	/* External sensors configuration
	 *	enable all the external sensors
	 *	set external sensor number to 0
	 *	set external sensor frequency to the max value: EXTFREQ = 000
	 *	don't divide the 8kHz input clock: DIVCLK = 000
	 *	lock the register
	 */
	if (external == 1) {
		for (i = 0; i < NUMBER_OF_EXT_SENS; i++) {
			if ((EXT_SENS_MASK & ext_conf) & (0x1 << i)) {
				status[i] = ARMED;
			}
		}
#ifndef _UNLOCK_
		SMON->EXTSCN = SMON_EXTSCN_LOCK_Msk + (EXT_SENS_MASK & ext_conf);
#else
		SMON->EXTSCN = (EXT_SENS_MASK & ext_conf);
#endif /* _UNLOCK_ */
	} else {
		SMON->EXTSCN = 0x00000000;
	}
	/*
	 *  Internal sensors configuration
	 *  bit 0: Shield
	 *  bit 1: Temp
	 *  bit 2: Vbat
	 */
	if (internal == 1) {
#ifndef _UNLOCK_
		SMON->INTSCN = 0x80000007 & (int_conf + SMON_EXTSCN_LOCK_Msk);
#else
		SMON->INTSCN = 0x00000007 & int_conf;
#endif
	} else {
		SMON->INTSCN = 0x00000000;
	}
	/* Generate and store AES key */
	if(load_aes_key())
		return COMMON_ERR_NOT_INITIALIZED;

	printf("\nRead of magic word at end of NVSRAM after load of AES key:\n");
	printf("0x%08X=0x%08X\n\n",(unsigned int)ptr_NVSRAM, (unsigned int)(*ptr_NVSRAM));

	return NO_ERROR;
}

int check_aes_key(void) {
	unsigned int data[4];

	/*
	 * check key erasing
	 * define AES data
	 */
	data[0] = 0x00000000;
	data[1] = 0x00000000;
	data[2] = 0x00000000;
	data[3] = 0x00000000;
	printf("AES key check\n");
	get_aes_result(data);
	/* if keywipe has been performed, the secure AES key is 0x0000000...000 */
	if ((data[3] == 0x2e2b34ca) && (data[2] == 0x59fa4c88)
			&& (data[1] == 0x3b2c8aef) && (data[0] == 0xd44be966)) {
		printf("\t### keywipe has been performed !!!###\n");
		return NO_ERROR;
	} else {
		printf("\tNo keywipe\n");
		return COMMON_ERR_FATAL_ERROR;
	}
}

int load_aes_key(void) {
	u8 aes_key[32];
	u32 result;
	u32 *ptr;
	u32 *ptr_key_register = (u32 *)0x40005000;

	/* Generate a pseudo random and store it into aes_key */
	result = sp80090_usage(&aes_key[0]);
	if(result)
		return result;

	ptr = (u32 *)&aes_key[0];

	/* Load aes_key */
	*ptr_key_register++ = ptr[0];
	*ptr_key_register++ = ptr[1];
	*ptr_key_register++ = ptr[2];
	*ptr_key_register++ = ptr[3];
	*ptr_key_register++ = ptr[4];
	*ptr_key_register++ = ptr[5];
	*ptr_key_register++ = ptr[6];
	*ptr_key_register   = ptr[7];

	return NO_ERROR;
}

void reload_system_after_drs(void) {
	printf("ACK DRS...\n");
	printf("Security alarm registers: 0x%08x\n", (unsigned int)SMON->SECALM);
	while (SMON->SECALM != 0x00000000) {
		/* Wait For access */
		wfa();
		/* Try to clear Security Alarm register */
		SMON->SECALM = 0x00000000;
		/* Wait For access */
		wfa();
	}
	printf("DRS has been detected\n");
	printf("DRS acknowledge\n");
	printf("Security alarm registers: 0x%08x\n", (unsigned int)SMON->SECALM);

	/* Must load at least one time AES Key with the TRNG in order to allowed cipher controller
	 * to read AES key from battery backed register at address 0x40005000 to 0x4000502F */
	/* Ask AES key transfer from TRNG to BB domain */
	TRNG->CN = TRNG_CN_AESKG_Msk;
	/* Wait until done */
	while ( TRNG->CN & TRNG_CN_AESKG_Msk);

	return;
}

int external_sens_tests(unsigned char *status, unsigned char *previous,
		int trace) {
	int result = COMMON_ERR_UNKNOWN;
	unsigned int secdiag;
	unsigned int i;

	if (trace == 1) {
		printf("DRS = 0x%08X\n", (unsigned int)SMON->DLRTC);
	}
	/* Read secure monitor status */
	if (trace == 1) {
		printf("External sensors registers: 0x%08x\n", (unsigned int)SMON->EXTSCN);
	}
	if (trace == 1) {
		printf("Internal sensors registers: 0x%08x\n", (unsigned int)SMON->INTSCN);
	}
	if (trace == 1) {
		printf("Security alarm registers: 0x%08x\n", (unsigned int)SMON->SECALM);
	}
	secdiag = SMON->SECDIAG;
	if (trace == 1) {
		printf("Security diagnostic registers: 0x%08x\n", secdiag);
	}

	for (i = 0; i < NUMBER_OF_EXT_SENS; i++) {
		if (secdiag & (1 << (i + SMON_SECDIAG_EXTSTAT0_Pos))) {
				status[i] = TRIGGERED;
				if (trace == 1) {
					printf("DS%d triggered\n", i);
				}
			}
	}

	for (i = 0; i < NUMBER_OF_EXT_SENS; i++) {
		if (status[i] != previous[i]) {
			result = COMMON_ERR_NO_MATCH;
		}
	}
	if (COMMON_ERR_NO_MATCH == result) {
		for (i = 0; i < NUMBER_OF_EXT_SENS; i++) {
			previous[i] = status[i];
		}
	}
	return result;
}

/******************************************************************************/
/* EOF */
