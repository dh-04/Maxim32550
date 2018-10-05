/*
 * @file MAX325xx.h
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

#ifndef MAX32550_CMSIS_DEVICE_H_
#define MAX32550_CMSIS_DEVICE_H_

#include "MAX32550.h"
#include "MAX32550_conf.h"
#include "cmsis_nvic.h"


/* =========================================================================================================================== */
/* ================                                   Interrupt Handlers                                      ================ */
/* =========================================================================================================================== */
void  PWF_IRQHandler(void);
void  WDG_IRQHandler(void);
void  USB_IRQHandler(void);
void  RTC_IRQHandler(void);
void  TRNG_IRQHandler(void);
void  TIMER0_IRQHandler(void);
void  TIMER1_IRQHandler(void);
void  TIMER2_IRQHandler(void);
void  TIMER3_IRQHandler(void);
void  TIMER4_IRQHandler(void);
void  TIMER5_IRQHandler(void);
void  SC_IRQHandler(void);
void  LCD_TFT_IRQHandler(void);
void  I2C_IRQHandler(void);
void  UART0_IRQHandler(void);
void  UART1_IRQHandler(void);
void  SPI0_IRQHandler(void);
void  SPI1_IRQHandler(void);
void  SPI2_IRQHandler(void);
void  SKBD_IRQHandler(void);
void  ADC_IRQHandler(void);
void  DAC_IRQHandler(void);
void  MSR_DSP_IRQHandler(void);
void  FLC_IRQHandler(void);
void  GPIO0_IRQHandler(void);
void  GPIO1_IRQHandler(void);
void  GPIO2_IRQHandler(void);
void  CRYPTO_IRQHandler(void);
void  DMA0_IRQHandler(void);
void  DMA1_IRQHandler(void);
void  DMA2_IRQHandler(void);
void  DMA3_IRQHandler(void);


#endif // MAX32550_CMSIS_DEVICE_H_
