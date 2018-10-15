/*
 * @file dump_otp/include/otp.h
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

#define	M_COBRA_MIN(_a_,_b_)				( ( _a_ > _b_ ) ? _b_ : _a_ )
#define	K_OTP_DUMP_WORKBUFFER_SIZE_BYTE			( 4 * 1024 )

#ifdef _OTP_DUMP_MAXIM_
#define	K_OTP_DUMP_BASE_ADDRESS					MML_MEM_OTP_MXIM_BASE
#define	K_OTP_DUMP_MAX_SIZE_BYTE				MML_MEM_OTP_MXIM_SIZE
#else
#define	K_OTP_DUMP_BASE_ADDRESS					MML_MEM_OTP_USER_BASE
#define	K_OTP_DUMP_MAX_SIZE_BYTE				MML_MEM_OTP_USER_SIZE
#endif /* _OTP_DUMP_MAXIM_ */
#define	K_OTP_DUMP_MAX_SIZE_INT					( K_OTP_DUMP_MAX_SIZE_BYTE / sizeof(unsigned int) )

/* Size of OTP line in bytes */
#define	K_OTP_DUMP_OTP_LINE_SIZE				0x08

/* Size of truncated CV into OTP line rounded to upper bytes value */
#define	K_OTP_DUMP_OTP_CV_LINE_SIZE				0x02

/**
 * @brief This function is used to initialize UART0 interface.
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 * @retval COMMON_ERR_OUT_OF_RANGE			Invalid UART port number
 * @retval COMMON_ERR_NULL_PTR				NULL pointer parameter passed
 * @retval COMMON_ERR_BAD_STATE				UART port is not initialized
 * @retval COMMON_ERR_INVAL					Invalid configuration parameter
 * @retval COMMON_ERR_RUNNING				UART is currently busy with transmitting or receiving the data
 * @retval MML_UART_ERR_IRQ_SET				Interrupt can not be initialized
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device identifier
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		Invalid parameter
 */
int init_uart(void);

/**
 * @brief This function is used to realign OTP data
 * @param[in] p_src							Pointer on the data to realign.
 * @param[in] length						Length of data to realign.
 */
void otp_dump_realign_data(unsigned char *p_src, unsigned int length);

/**
 * @brief This function is used to dump on display OTP data.
 * @param[in] address						Start address.
 * @param[in] length						length to dump.
 * @param[in] otp_id						OTP area identifier.
 * @retval NO_ERROR							No error.
 */
int iflash_dump_display(unsigned int address, unsigned int length, mml_sflc_otp_id_t otp_id);

/**
 * @brief This function is used to initialize flash controller and UART.
 * @retval NO_ERROR							No error.
 * @retval COMMON_ERR_UNKNOWN				Unknown error.
 * @retval COMMON_ERR_OUT_OF_RANGE			Invalid UART port number.
 * @retval COMMON_ERR_NULL_PTR				NULL pointer parameter passed.
 * @retval COMMON_ERR_BAD_STATE				UART port is not initialized.
 * @retval COMMON_ERR_INVAL					Invalid configuration parameter.
 * @retval COMMON_ERR_RUNNING				UART is currently busy with transmitting or receiving the data.
 * @retval MML_UART_ERR_IRQ_SET				Interrupt can not be initialized.
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device identifier.
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		Invalid parameter.
 */
int iflash_dump_init(void) ;
