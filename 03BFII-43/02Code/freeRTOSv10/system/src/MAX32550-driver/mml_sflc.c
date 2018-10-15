/*
 * @file mml_sflc.c MML SFLC Driver C File
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

#include<MAX325xx.h>

#ifdef _MML_SFLC_H_

#include <errors.h>
#include <string.h>

#define SFLC_PEND	(FC_FLSH_CN_PEND_Msk | FC_FLSH_CN_PGE_Msk)
unsigned char mml_sflc_buffer[K_MML_SFLC_ERASE_PAGE_SIZE];

void mml_sflc_init(void) {
	memset(mml_sflc_buffer, K_MML_SFLC_OTP_VIRGIN_PATTERN_BYTE,
			sizeof(mml_sflc_buffer));
}

void mml_sflc_shutdown(void) {
	memset(mml_sflc_buffer, K_MML_SFLC_OTP_VIRGIN_PATTERN_BYTE,
			sizeof(mml_sflc_buffer));
}

/* Generic ********************************************************************/

void mml_sflc_otp_unlock(void) {
	/* Wait for flash controller to release busy bit - set to '1' */
	while ( FC->FLSH_CN & ( FC_FLSH_ALL_CMDS_Msk | FC_FLSH_CN_PEND_Msk));
	/* Write in OLOCK the 1st magic value */
	FC->FLSH_ACNTL = (unsigned int) FL_ACNTL_MAGIC_WORD1;
	/* Write in OLOCK the 2nd magic value */
	FC->FLSH_ACNTL = (unsigned int) FL_ACNTL_MAGIC_WORD2;
	/* Write in OLOCK the 3rd magic value */
	FC->FLSH_ACNTL = (unsigned int) FL_ACNTL_MAGIC_WORD3;
	/* Wait for flash controller to release busy bit - set to '1' */
	while ( FC->FLSH_CN & ( FC_FLSH_ALL_CMDS_Msk | FC_FLSH_CN_PEND_Msk));
}

void mml_sflc_otp_lock(void) {
	/* Wait for flash controller to release busy bit - set to '1' */
	while ( FC->FLSH_CN & ( FC_FLSH_ALL_CMDS_Msk | FC_FLSH_CN_PEND_Msk));
	/* Write any value but magic word */
	FC->FLSH_ACNTL = 0;
	/* Wait for flash controller to release busy bit - set to '1' */
	while ( FC->FLSH_CN & ( FC_FLSH_ALL_CMDS_Msk | FC_FLSH_CN_PEND_Msk));
}

int mml_sflc_write(unsigned int address, unsigned char* tx_buf, unsigned int length) {
	unsigned int len_128, len_32;
	unsigned int offset_128, offset_32, address_32;
	unsigned char tab[4];
	int ret;

	/* Check input pointers */
	if (!tx_buf) {
		return COMMON_ERR_NULL_PTR;
	}

	/* check length */
	if (!length) {
		return COMMON_ERR_INVAL;
	}

	/* Check input parameters */
	if ((address < MML_MEM_FLASH_BASE) | (address > (MML_MEM_FLASH_BASE + MML_MEM_FLASH_SIZE))) {
		return N_MML_SFLC_ERR_OVERFLOW;
	}

	/* Overflow  */
	if ((address + length) > (MML_MEM_FLASH_BASE + MML_MEM_FLASH_SIZE)) {
		return N_MML_SFLC_ERR_OVERFLOW;
	}

	offset_128 = address & ~K_MML_SFLC_PGM_128BITS_SPECIAL_MASK;
	if (offset_128) {
		/* Not 128 bits aligned */
		len_128 = K_MML_SFLC_PGM_128BITS_LENGHT - offset_128;
		if (len_128 > length)
			len_128 = length;
		offset_32 = address & ~K_MML_SFLC_PGM_32BITS_SPECIAL_MASK;
		if (offset_32) {
			/* Not 32 bits aligned */
			address_32 = (address & K_MML_SFLC_PGM_32BITS_SPECIAL_MASK);
			memcpy(tab, (unsigned char *)address_32, K_MML_SFLC_PGM_32BITS_LENGHT);
			len_32 = K_MML_SFLC_PGM_32BITS_LENGHT - offset_32;
			if (len_32 > len_128) {
				memset(&tab[offset_32+len_128],0xFF,len_32 - len_128);
				len_32 = len_128;
			}
			memcpy(&tab[offset_32], tx_buf, len_32);
			if ((ret = mml_sflc_write_32bits(address_32, (unsigned int *)tab)) != NO_ERROR)
				 return ret;
			address = address_32 + K_MML_SFLC_PGM_32BITS_LENGHT;
			length -= len_32;
			tx_buf += len_32;
			len_128 -= len_32;
		}
		while (len_128 >= K_MML_SFLC_PGM_32BITS_LENGHT) {
			if ((ret = mml_sflc_write_32bits(address, (unsigned int *)tx_buf)) != NO_ERROR)
				 return ret;
			address += K_MML_SFLC_PGM_32BITS_LENGHT;
			tx_buf += K_MML_SFLC_PGM_32BITS_LENGHT;
			length -= K_MML_SFLC_PGM_32BITS_LENGHT;
			len_128 -= K_MML_SFLC_PGM_32BITS_LENGHT;
		}
	}

#ifdef __SFLC_BURST
	if (length >= ( 2*K_MML_SFLC_PGM_128BITS_LENGHT)) {
		/* Burst */
		if ((ret = mml_sflc_write_128bits_burst_start(address,tx_buf)) != NO_ERROR) {
			mml_sflc_lock_flash();
			return ret;
		}
		address += K_MML_SFLC_PGM_128BITS_LENGHT;
		tx_buf += K_MML_SFLC_PGM_128BITS_LENGHT;
		length -= K_MML_SFLC_PGM_128BITS_LENGHT;
		while (length >= K_MML_SFLC_PGM_128BITS_LENGHT) {
			if ((ret = mml_sflc_write_128bits_burst_continue(address, tx_buf)) != NO_ERROR) {
			 mml_sflc_lock_flash();
			 return ret;
			}
			address += K_MML_SFLC_PGM_128BITS_LENGHT;
			tx_buf += K_MML_SFLC_PGM_128BITS_LENGHT;
			length -= K_MML_SFLC_PGM_128BITS_LENGHT;
		}
		mml_sflc_lock_flash();
	} else {
		if (length >= K_MML_SFLC_PGM_128BITS_LENGHT) {
			if ((ret = mml_sflc_write_128bits(address, tx_buf)) != NO_ERROR)
				 return ret;
			 address += K_MML_SFLC_PGM_128BITS_LENGHT;
			 tx_buf += K_MML_SFLC_PGM_128BITS_LENGHT;
			 length -= K_MML_SFLC_PGM_128BITS_LENGHT;
		}
	}
#else
	 while (length >= K_MML_SFLC_PGM_128BITS_LENGHT) {
		 if ((ret = mml_sflc_write_128bits(address, (unsigned int *)tx_buf)) != NO_ERROR)
			 return ret;
		 address += K_MML_SFLC_PGM_128BITS_LENGHT;
		 tx_buf += K_MML_SFLC_PGM_128BITS_LENGHT;
		 length -= K_MML_SFLC_PGM_128BITS_LENGHT;
	 }
#endif
	 while (length >= K_MML_SFLC_PGM_32BITS_LENGHT) {
		 if ((ret = mml_sflc_write_32bits(address, (unsigned int *)tx_buf)) != NO_ERROR)
			 return ret;
		 address += K_MML_SFLC_PGM_32BITS_LENGHT;
		 tx_buf += K_MML_SFLC_PGM_32BITS_LENGHT;
		 length -= K_MML_SFLC_PGM_32BITS_LENGHT;
	 }
	 if (length) {
		memcpy(tab, (unsigned char *)address, K_MML_SFLC_PGM_32BITS_LENGHT);
		memcpy(tab, tx_buf, length);
		if ((ret = mml_sflc_write_32bits(address, (unsigned int *)tab)) != NO_ERROR)
			 return ret;
	 }

	return NO_ERROR;
}


/*************************************************************************
 * Flash
 *************************************************************************/
int mml_sflc_erase(unsigned int address, unsigned int length){
	int ret;
	unsigned int address_sector_start,address_sector_start_1, address_sector_end;

	/* check length */
	if (!length) {
		return COMMON_ERR_INVAL;
	}

	/* Check input parameters */
	if ((address < MML_MEM_FLASH_BASE) | (address > (MML_MEM_FLASH_BASE + MML_MEM_FLASH_SIZE))) {
		return N_MML_SFLC_ERR_OVERFLOW;
	}

	if ((address + length) > (MML_MEM_FLASH_BASE + MML_MEM_FLASH_SIZE)) {
		return N_MML_SFLC_ERR_OVERFLOW;
	}

	address_sector_start = address & K_MML_SFLC_ERASE_PAGE_SPECIAL_MASK;
	address_sector_start_1 = address_sector_start + K_MML_SFLC_ERASE_PAGE_SIZE;
	address_sector_end = (address + length) & K_MML_SFLC_ERASE_PAGE_SPECIAL_MASK;

	if( address_sector_start_1 < address_sector_end) {
		if ((ret = mml_sflc_erase_sector(address, address_sector_start_1 - address)) != NO_ERROR)
				 return ret;
		if ((ret = mml_sflc_erase_sector(address_sector_start_1, address_sector_end - address_sector_start_1)) != NO_ERROR)
			 return ret;
		if ((ret = mml_sflc_erase_sector(address_sector_end, address + length - address_sector_end)) != NO_ERROR)
			 return ret;
	} else {
		if ((ret = mml_sflc_erase_sector(address, length)) != NO_ERROR)
			 return ret;
	}

	return NO_ERROR;
}

int mml_sflc_erase_sector(unsigned int address, unsigned int length)
{
	int ret;
	unsigned int	sector_offset = address & K_MML_SFLC_ERASE_PAGE_SPECIAL_MASK;

	if(sector_offset != address) {
		/* Copy CRYPTO_BLOCK_SIZE byte aligned */
		memcpy(mml_sflc_buffer, (unsigned char *)(sector_offset), (address - sector_offset));

		/* Check if (address + length) is less than a sector size */
		if ( (address + length) < (sector_offset + K_MML_SFLC_ERASE_PAGE_SIZE) ) {
			memcpy(mml_sflc_buffer + ((address + length) - sector_offset),
					(unsigned char *)(address + length),
					sector_offset + K_MML_SFLC_ERASE_PAGE_SIZE - (address + length));
			 /* 1: Erase sector */
			 if ((ret = mml_sflc_erase_full_sector(sector_offset)) != NO_ERROR)
				 return ret;

			 /* 2: write mml_sflc_buffer from sector_offset to address */
			 if ((ret = mml_sflc_write(sector_offset, mml_sflc_buffer, (address - sector_offset))) != NO_ERROR)
				 return ret;

			 /* 3: write mml_sflc_buffer from (address + length) to (sector_offset + K_MML_SFLC_ERASE_PAGE_SIZE) */
			 if ((ret = mml_sflc_write(sector_offset  + ((address + length) - sector_offset),
					 mml_sflc_buffer  + ((address + length) - sector_offset),
					 sector_offset + K_MML_SFLC_ERASE_PAGE_SIZE - (address + length))) != NO_ERROR)
				 return ret;
			return NO_ERROR;
		} else {
			 /* 1: Erase sector */
			 if ((ret = mml_sflc_erase_full_sector(sector_offset)) != NO_ERROR)
				 return ret;

			 /* 2: write mml_sflc_buffer from sector_offset to address */
			 if ((ret = mml_sflc_write(sector_offset, mml_sflc_buffer, (address - sector_offset))) != NO_ERROR)
				 return ret;

			 length -= (K_MML_SFLC_ERASE_PAGE_SIZE - (address & ~K_MML_SFLC_ERASE_PAGE_SPECIAL_MASK));
			address = sector_offset + K_MML_SFLC_ERASE_PAGE_SIZE;
		}
	}

	while (length >= K_MML_SFLC_ERASE_PAGE_SIZE) {
		 /* Erase sector(s) */
		 if ((ret = mml_sflc_erase_full_sector(address)) != NO_ERROR)
			 return ret;
		length -= K_MML_SFLC_ERASE_PAGE_SIZE;
		address += K_MML_SFLC_ERASE_PAGE_SIZE;
	}

	/* Last sectors */
	if (length) {
		memcpy(mml_sflc_buffer + length, (unsigned char *)(address + length), K_MML_SFLC_ERASE_PAGE_SIZE - length);
		 /* 1: Erase sector */
		 if ((ret = mml_sflc_erase_full_sector(address)) != NO_ERROR)
			 return ret;
		 /* 2: write address+length from sector_offset+leng to end of sector */
		 if ((ret = mml_sflc_write(address + length, mml_sflc_buffer + length, K_MML_SFLC_ERASE_PAGE_SIZE - length)) != NO_ERROR)
			 return ret;
	}

	return NO_ERROR;
}

int  __attribute__ ((section(".ram_code"))) mml_sflc_erase_full_sector(unsigned int address) {
	int ret = NO_ERROR;
	volatile unsigned int val;
#ifdef _WITH_FORCED_EXIT_
	int forced_loop = 10000;
#endif /*_WITH_FORCED_EXIT_ */

	if ((address < MML_MEM_FLASH_BASE) | (address > (MML_MEM_FLASH_BASE + MML_MEM_FLASH_SIZE - K_MML_SFLC_ERASE_PAGE_SIZE))) {
		return N_MML_SFLC_ERR_OVERFLOW;
	}

	if (address != ( address & K_MML_SFLC_ERASE_PAGE_SPECIAL_MASK))	{
		return N_MML_SFLC_ERR_OVERFLOW;
	}

	/* Wait for end of process - if any */
	while (FC->FLSH_CN & FC_FLSH_CN_PEND_Msk);

	/* Clear out interrupt - if any */
	FC->FLSH_INT = 0;

	/* Unlock flash and set erase mode */
	FC->FLSH_CN |= ((FC_CN_UNLOCK_unlocked << FC_FLSH_CN_UNLOCK_Pos) | (FC_CN_ERASE_CODE_erasePage << FC_FLSH_CN_ERASE_CODE_Pos));
	/* Wait for end of process - if any */
	while (FC->FLSH_CN & FC_FLSH_CN_PEND_Msk);

	/* Set address */
	FC->FLSH_ADDR = address;

	/* Set command - Unlock inside */
	FC->FLSH_CN |= (FC_CN_PGE_start << FC_FLSH_CN_PGE_Pos);
	/* Wait for end of process */
#ifdef _WITH_FORCED_EXIT_
	while( ( FC->FLSH_CN & ( FC_FLSH_CN_PEND_Msk | FC_FLSH_CN_PGE_Msk ) ) && forced_loop-- );
	if (forced_loop == 0) {
		/* Lock flash and remove erase mode */
		ret = COMMON_ERR_IN_PROGRESS;
	}
#else
	while (FC->FLSH_CN & FC_FLSH_CN_PGE_Msk);
#endif

	/* check Status */
	val = FC->FLSH_INT;
	if ((!(val & FC_FLSH_INT_DONE_Msk)) || (val & FC_FLSH_INT_AF_Msk)) {
		ret = N_MML_SFLC_ERR_PAGE_ERASE_FAILED;
	}

	/* Lock flash and remove erase mode */
	FC->FLSH_CN &= ~(FC_FLSH_CN_UNLOCK_Msk | FC_FLSH_CN_ERASE_CODE_Msk);
	while (FC->FLSH_CN & FC_FLSH_CN_PEND_Msk);

	return ret;
}

int mml_sflc_mass_erase(void) {
	int ret = NO_ERROR;
	volatile unsigned int val;
#ifdef _WITH_FORCED_EXIT_
	int forced_loop = 10000;
#endif /*_WITH_FORCED_EXIT_ */

	/* Wait for end of process - if any */
	while (FC->FLSH_CN & FC_FLSH_CN_PEND_Msk);

	/* Clear out interrupt - if any */
	FC->FLSH_INT = 0;

	/* Unlock flash and set erase mode */
	FC->FLSH_CN |= ((FC_CN_UNLOCK_unlocked << FC_FLSH_CN_UNLOCK_Pos) | (FC_CN_ERASE_CODE_eraseAll << FC_FLSH_CN_ERASE_CODE_Pos));
	/* Wait for end of process - if any */
	while (FC->FLSH_CN & FC_FLSH_CN_PEND_Msk);

	/* Set address */
	FC->FLSH_ADDR = MML_MEM_FLASH_BASE;
	/* Set command - Unlock inside */
	FC->FLSH_CN |= FC_FLSH_CN_ME_Msk;
	/* Wait for end of process */

#ifdef _WITH_FORCED_EXIT_
	while( ( FC->FLSH_CN & ( FC_FLSH_CN_PEND_Msk | FC_FLSH_CN_ME_Msk ) ) && forced_loop-- );
	if (forced_loop == 0)
		return COMMON_ERR_IN_PROGRESS;
#else
	while (FC->FLSH_CN & FC_FLSH_CN_PEND_Msk);
#endif /* _WITH_FORCED_EXIT_ */
	/* check Status */
	val = FC->FLSH_INT;
	if ((!(val & FC_FLSH_INT_DONE_Msk)) || (val & FC_FLSH_INT_AF_Msk)) {
		ret = N_MML_SFLC_ERR_PAGE_ERASE_FAILED;
	}

	/* Lock flash and remove erase mode */
	FC->FLSH_CN &= ~(FC_FLSH_CN_UNLOCK_Msk | FC_FLSH_CN_ERASE_CODE_Msk);
	while (FC->FLSH_CN & FC_FLSH_CN_PEND_Msk);

	return ret;
}

int  __attribute__ ((section(".ram_code"))) mml_sflc_write_32bits(unsigned int address, unsigned int *p_src) {
	int ret = NO_ERROR;
	volatile unsigned int val;

	/* Wait for end of process - if any */
	while (FC->FLSH_CN & FC_FLSH_CN_PEND_Msk);

	/* Clear out interrupt - if any */
	FC->FLSH_INT = 0;

	/* Unlock flash - 128bits mode by default */
	FC->FLSH_CN |= ((FC_CN_UNLOCK_unlocked << FC_FLSH_CN_UNLOCK_Pos)
				| (FC_CN_WDTH_size32 << FC_FLSH_CN_WDTH_Pos));

	/* Set address - It has to be integer aligned. "Byte" Address has to
	 * be converted into "Integer" address by being divided by 4 */
	FC->FLSH_ADDR = address;
	/* Set data */
	FC->FLSH_DATA0 = p_src[0];

	/* Set command - Unlock inside */
	FC->FLSH_CN |= (FC_CN_WR_start << FC_FLSH_CN_WR_Pos);
	/* Wait for end of process */
	while (FC->FLSH_CN & FC_FLSH_CN_PEND_Msk);

	/* check Status */
	val = FC->FLSH_INT;
	if ((!(val & FC_FLSH_INT_DONE_Msk)) || (val & FC_FLSH_INT_AF_Msk)) {
		ret = N_MML_SFLC_ERR_PAGE_ERASE_FAILED;
	}

	/* Clear out interrupt */
	FC->FLSH_INT = 0;

	/* Lock flash and remove erase mode */
	FC->FLSH_CN &= ~(FC_FLSH_CN_UNLOCK_Msk | FC_FLSH_CN_ERASE_CODE_Msk);
	while (FC->FLSH_CN & FC_FLSH_CN_PEND_Msk);

	return ret;
}

int __attribute__ ((section(".ram_code"))) mml_sflc_write_128bits(unsigned int address, unsigned int *p_src) {
	int ret = NO_ERROR;
	volatile unsigned int val;

	/* Wait for end of process - if any */
	while (FC->FLSH_CN & FC_FLSH_CN_PEND_Msk);

	/* Clear out interrupt - if any */
	FC->FLSH_INT = 0;

	/* Unlock flash - 128bits mode by default */
	FC->FLSH_CN |= (FC_CN_UNLOCK_unlocked << FC_FLSH_CN_UNLOCK_Pos);
	FC->FLSH_CN &= ~(FC_FLSH_CN_WDTH_Msk);

	/* Set address - It has to be integer aligned. "Byte" Address has to
	 * be converted into "Integer" address by being divided by 4 */
	FC->FLSH_ADDR = address;

	/* Set data */
	FC->FLSH_DATA0 = p_src[0];
	FC->FLSH_DATA1 = p_src[1];
	FC->FLSH_DATA2 = p_src[2];
	FC->FLSH_DATA3 = p_src[3];

	/* Set command - Unlock inside */
	FC->FLSH_CN |= (FC_CN_WR_start << FC_FLSH_CN_WR_Pos);

	/* Wait for end of process */
	while (FC->FLSH_CN & FC_FLSH_CN_PEND_Msk);

	/* check Status */
	val = FC->FLSH_INT;
	if ((!(val & FC_FLSH_INT_DONE_Msk)) || (val & FC_FLSH_INT_AF_Msk)) {
		ret = N_MML_SFLC_ERR_PAGE_ERASE_FAILED;
	}

	/* Clear out interrupt */
	FC->FLSH_INT = 0;

	/* Lock flash and remove erase mode */
	FC->FLSH_CN &= ~(FC_FLSH_CN_UNLOCK_Msk | FC_FLSH_CN_ERASE_CODE_Msk);
	while (FC->FLSH_CN & FC_FLSH_CN_PEND_Msk);

	return ret;
}

/* OTP ************************************************************************/

int mml_sflc_otp_write(mml_sflc_otp_id_t id, unsigned int address,
		unsigned char *p_src, unsigned int length) {
	int ret = COMMON_ERR_UNKNOWN;

	/* Check input pointers */
	if (!p_src) {
		return COMMON_ERR_NULL_PTR;
	}

	/* check length */
	if (!length) {
		return COMMON_ERR_INVAL;
	}

	/* Check that address is integer aligned */
	if (address & ~K_MML_SFLC_PGM_32BITS_SPECIAL_MASK) {
		return N_MML_SFLC_ERR_ADDRESS_NOT_ALIGNED;
	}

	/* Check length alignment */
	if (length & ~K_MML_SFLC_PGM_32BITS_SPECIAL_MASK) {
		return N_MML_SFLC_ERR_LENGTH_NOT_ALIGNED;
	}

	switch (id) {
	case N_MML_SFLC_OTP_ID_MAXIM:
		if ((address < K_MML_SFLC_OTP_MAXIM_BASE) | (address > (K_MML_SFLC_OTP_MAXIM_BASE + K_MML_SFLC_OTP_MAXIM_SIZE))) {
			return N_MML_SFLC_ERR_OVERFLOW;
		}

		if ((address + length) > (K_MML_SFLC_OTP_MAXIM_BASE + K_MML_SFLC_OTP_MAXIM_SIZE)) {
			return N_MML_SFLC_ERR_OVERFLOW;
		}
		break;
	case N_MML_SFLC_OTP_ID_USER:
		if ((address < K_MML_SFLC_OTP_USER_BASE) | (address > (K_MML_SFLC_OTP_USER_BASE + K_MML_SFLC_OTP_USER_SIZE))) {
			return N_MML_SFLC_ERR_OVERFLOW;
		}

		if ((address + length) > (K_MML_SFLC_OTP_USER_BASE + K_MML_SFLC_OTP_USER_SIZE)) {
			return N_MML_SFLC_ERR_OVERFLOW;
		}
		break;
	default:
		return COMMON_ERR_INVAL;
		break;
	}

	/* Unlock - contains BUSY bit treatment */
	mml_sflc_otp_unlock();

	while (length) {
		if ((ret = mml_sflc_write_32bits(address, (unsigned int *)p_src)) != NO_ERROR)
			 break;
		address += K_MML_SFLC_PGM_32BITS_LENGHT;
		length -= K_MML_SFLC_PGM_32BITS_LENGHT;
		p_src += K_MML_SFLC_PGM_32BITS_LENGHT;
	}

	/* Lock - contains BUSY bit treatment */
	mml_sflc_otp_lock();

	return ret;
}

int mml_sflc_otp_read(mml_sflc_otp_id_t id, unsigned int address,
		unsigned char *p_dst, unsigned int length) {
	int result = COMMON_ERR_UNKNOWN;

	/* Check input pointers */
	if (!p_dst) {
		return COMMON_ERR_NULL_PTR;
	}

	/* check length */
	if (!length) {
		return COMMON_ERR_INVAL;
	}

	/* Check that address is integer aligned */
	if (address & ~K_MML_SFLC_PGM_32BITS_SPECIAL_MASK) {
		return N_MML_SFLC_ERR_ADDRESS_NOT_ALIGNED;
	}

	/* Check length alignment */
	if (length & ~K_MML_SFLC_PGM_32BITS_SPECIAL_MASK) {
		return N_MML_SFLC_ERR_LENGTH_NOT_ALIGNED;
	}

	switch (id) {
	case N_MML_SFLC_OTP_ID_MAXIM:
		if ((address < K_MML_SFLC_OTP_MAXIM_BASE) | (address > (K_MML_SFLC_OTP_MAXIM_BASE + K_MML_SFLC_OTP_MAXIM_SIZE))) {
			return N_MML_SFLC_ERR_OVERFLOW;
		}

		if ((address + length) > (K_MML_SFLC_OTP_MAXIM_BASE + K_MML_SFLC_OTP_MAXIM_SIZE)) {
			return N_MML_SFLC_ERR_OVERFLOW;
		}
		break;
	case N_MML_SFLC_OTP_ID_USER:
		if ((address < K_MML_SFLC_OTP_USER_BASE) | (address > (K_MML_SFLC_OTP_USER_BASE + K_MML_SFLC_OTP_USER_SIZE))) {
			return N_MML_SFLC_ERR_OVERFLOW;
		}

		if ((address + length) > (K_MML_SFLC_OTP_USER_BASE + K_MML_SFLC_OTP_USER_SIZE)) {
			return N_MML_SFLC_ERR_OVERFLOW;
		}
		break;
	default:
		return COMMON_ERR_INVAL;
		break;
	}

	/* Unlock - contains BUSY bit treatment */
	mml_sflc_otp_unlock();
	/* Read data */
	memcpy(p_dst, (unsigned char *) address, length);
	/* Lock - contains BUSY bit treatment */
	mml_sflc_otp_lock();
	result = NO_ERROR;

	return result;
}

const char * mml_sflc_get_version(void) {
	return MML_SFLC_VERSION_STRING;
}

#endif //_MML_SFLC_H_

/******************************************************************************/
/* EOF */

