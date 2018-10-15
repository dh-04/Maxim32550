/*
 * @file mml_sc.c MML SC Driver C File
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

#include <MAX325xx.h>

#ifdef _MML_SC_H_

#include <errors.h>

/** This structure is located in bss section, therefore it shall be zero-ized
 * at the very beginning of the application */
__attribute__((section(".bss")))          mml_sc_context_t sc_context;

int mml_sc_init(mml_sc_id_t id) {
	/* For the first time initialization */
	if (!sc_context.first_init) {
		/* Initialize the smart card context information to zero's */
		sc_context.sc[MML_SC_DEV0].reg_sc = SC;
#ifdef _SC_RESET_AT_INIT_
		/* Reset smart card 0 interface */
		GCR->RSTR |= GCR_RSTR_SC_Msk;
		/* Wait for smart card 0 reset to be done */
		while( GCR_RSTR_SC_Msk & GCR->RSTR );
		/* Stop clock of each TIMER. It'll be set independently */
		GCR->PERCKCN |= GCR_PERCKCN_SCD_Msk;
#endif /* _SC_RESET_AT_INIT_ */
		sc_context.first_init = 1;
	}
	/* Check input parameter */
	if (id > MML_SC_DEV_MAX) {
		return COMMON_ERR_INVAL;
	}
	/* Enable the timer clock */
	/* Clear the bit position to enable clock to timer device */
	switch (id) {
	case MML_SC_DEV0:
		GCR->PERCKCN &= ~GCR_PERCKCN_SCD_Msk;
		break;
	default:
		return COMMON_ERR_INVAL;
	}
	return NO_ERROR;
}

const char * mml_sc_get_version(void) {
	return MML_SC_VERSION_STRING;
}

#endif //_MML_SC_H_

/* EOF */
