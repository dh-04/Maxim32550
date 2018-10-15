/*
 * @file i2c/include/i2c.h
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
 * @brief This function is used to read data from a slave.
 * @param[in] register_adress			Address to read.
 * @param[out] data						Pointer on the location where the read data will be stored.
 * @param[in] plength					Pointer on the length of value to receive in bytes (maximum 8).
 * @retval NO_ERROR						No error.
 * @retval MML_I2C_ERR_INIT				Device is not initialized.
 * @retval COMMON_ERR_NULL_PTR			NULL pointer parameter passed.
 * @retval COMMON_ERR_UNKNOWN			Unknown error.
 * @retval MML_I2C_ERR_READY			TX FIFO is not empty.
 * @retval COMMON_ERR_INVAL				No such device or value is not appropriate.
 * @retval MML_I2C_ERR_INVALID_PARAM	Error on plength..
 * @retval MML_I2C_ERR_INVALID_ADDRESS	Address is not one of 10 bit OR 7 bit pattern.
 */
int i2c_read(unsigned char *register_adress, unsigned char *data, unsigned int *plength);

/**
 * @brief This function is used to write data to a slave.
 * @param[in] register_adress			Address to read.
 * @param[out] data						Pointer on the location where the read data will be stored.
 * @param[in] plength					Pointer on the length of value to write in bytes (maximum 8).
 * @retval NO_ERROR						No error.
 * @retval MML_I2C_ERR_INIT				Device is not initialized.
 * @retval COMMON_ERR_NULL_PTR			NULL pointer parameter passed.
 * @retval COMMON_ERR_UNKNOWN			Unknown error.
 * @retval MML_I2C_ERR_READY			TX FIFO is not empty.
 * @retval COMMON_ERR_INVAL				No such device or value is not appropriate.
 * @retval MML_I2C_ERR_INVALID_ADDRESS	Address is not one of 10 bit OR 7 bit pattern.
 */
int i2c_write(unsigned char *data, unsigned int *plength);
