/*
 * @file blink_led/src/main.c
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
 * @defgroup EX_BLINK_LED Blink LED Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example show you how to configure GPIO in order to blink LED1.
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */


#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include <MAX325xx.h>
#include "mlsLCDDriver.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"


#define EX_BLINK_LED_VERS_MAJOR	2
#define EX_BLINK_LED_VERS_MINOR	0
#define EX_BLINK_LED_VERS_PATCH	1
#define EX_BLINK_LED_VERSION_STRING	"v" xstr(EX_BLINK_LED_VERS_MAJOR) "." xstr(EX_BLINK_LED_VERS_MINOR) "." xstr(EX_BLINK_LED_VERS_PATCH)


#define LED1_GPIO_PORT	MML_GPIO_DEV0
#define LED1_GPIO_PIN	21
#define DELAY_TIME		1000

xSemaphoreHandle xBinarySemaphore;

void vTask1(void *pvParameters );
void vTaskCheckRunTimeNewCode( Int8 * szSourceSring );

/*NOTE:
 * Due to not use pvPortMalloc for allocate memory for New Code, so do not calculate the allocate memory for New Code
 * Calculate memory New Code by calculating the size of array QRCodeBufeer and tempBuffer.
 */

/*NOTE:
 * Check runtime by function xTaskGetTickCount after vTaskStartScheduler() run.
 */

static void CheckMemoryOldCode();
static void CheckRunTimeOldCode();

void __attribute__ ((noinline))  __attribute__((optimize("-O0"))) 
loop_delay(unsigned int ms)
{
     unsigned int i = ms*5000;
     unsigned int systemfreq = 0;
     mml_get_system_frequency(&systemfreq);

     i /= (MML_SYSTEM_FREQUENCY_108_MHZ/systemfreq);

     for (; i != 0 ; i--)
          __asm volatile ("nop\n");

}

int main(void)
{
	size_t preHeapSize, nowHeapSize, AllocatedHeapSize, leakHeapSize;
	TickType_t xTime1, xTime2, xExecutionTime;


	mlsLCDInit();
	mlsLCDDrawScreen(~LCD_WHITE);

	CheckMemoryOldCode();

	// check total memory allocated on heap of OLDCODE
	AllocatedHeapSize = xPortGetMinimumEverFreeHeapSize();

	vSemaphoreCreateBinary(xBinarySemaphore);
	if(  xBinarySemaphore != NULL)
	{
		xTaskCreate( vTask1, "Task 1",100, NULL, 1,NULL );
		xTaskCreate( vTaskCheckRunTimeNewCode, "Task 2",2500, "LeDTruong", 2,NULL );
		vTaskStartScheduler();
	}

	while (1) {
	}
}

void vTask1( void *pvParameters )
{
	for( ;; )
	{
		CheckRunTimeOldCode();
		xSemaphoreGive(xBinarySemaphore);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void vTaskCheckRunTimeNewCode( Int8 * szSourceSring )
{
	UInt8 QRCodeBufeer[qrcodegen_BUFFER_LEN_MAX];
	UInt8 tempBuffer[qrcodegen_BUFFER_LEN_MAX];
	TickType_t xTime1, xTime2, xExecutionTime;
	for( ;; )
	{
		xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
		xTime1 = xTaskGetTickCount();
		mlsLCDDisplayCustomQRCode(szSourceSring, QRCodeBufeer, tempBuffer);
		xTime2 = xTaskGetTickCount();
		xExecutionTime = xTime2 - xTime1;
	}
}

static void CheckMemoryOldCode()
{
	size_t preHeapSize, nowHeapSize, AllocatedHeapSize, leakHeapSize;
	//check memory leak of OLDCODE
	preHeapSize = xPortGetFreeHeapSize();
	_OldmlsLCDDisplayCustomQRCode("LeDTruong");
	nowHeapSize = xPortGetFreeHeapSize();
	leakHeapSize = nowHeapSize - preHeapSize;

	// check total memory allocated on heap of OLDCODE
	AllocatedHeapSize = xPortGetMinimumEverFreeHeapSize();

}

static void CheckRunTimeOldCode()
{
	TickType_t xTime1, xTime2, xExecutionTime;
	xTime1 = xTaskGetTickCount();
	_OldmlsLCDDisplayCustomQRCode("LeDTruong");
	xTime2 = xTaskGetTickCount();
	xExecutionTime = xTime2 - xTime1;
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__


