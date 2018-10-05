//
// This file is part of the µOS++ III distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include "cortexm/ExceptionHandlers.h"

// ----------------------------------------------------------------------------

void __attribute__((weak))
Default_Handler(void);

/* Forward declaration of the specific IRQ handlers. These are aliased
 to the Default_Handler, which is a 'forever' loop. When the application
 defines a handler (with the same name), this will automatically take
 precedence over these weak definitions */

void __attribute__ ((weak, alias ("Default_Handler"))) PWF_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) WDG_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) USB_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) RTC_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) TRNG_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) TIMER0_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) TIMER1_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) TIMER2_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) TIMER3_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) TIMER4_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) TIMER5_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) SC_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) LCD_TFT_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) I2C_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) UART0_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) UART1_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) SPI0_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) SPI1_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) SPI2_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) SKBD_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) ADC_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) DAC_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) MSR_DSP_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) FLC_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) GPIO0_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) GPIO1_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) GPIO2_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) CRYPTO_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) DMA0_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) DMA1_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) DMA2_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler"))) DMA3_IRQHandler(void);








// ----------------------------------------------------------------------------

extern unsigned int _estack;

typedef void
(* const pHandler)(void);

// ----------------------------------------------------------------------------

// The vector table.
extern void xPortPendSVHandler( void );
extern void xPortSysTickHandler( void );
extern void vPortSVCHandler( void );
// This relies on the linker script to place at correct location in memory.

__attribute__ ((section(".isr_vector"),used))
pHandler g_pfnVectors[] =
  {
	  /* -------------------     Cortex M3 Core IRQ  ----------------------------------*/
      (pHandler) &_estack, // The initial stack pointer
      Reset_Handler, // The reset handler
      NMI_Handler, // The NMI handler
      HardFault_Handler, // The hard fault handler
      MemManage_Handler,                        // The MPU fault handler
      BusFault_Handler,                        // The bus fault handler
      UsageFault_Handler,                        // The usage fault handler
      0,                                        // Reserved
      0,                                        // Reserved
      0,                                        // Reserved
      0,                                        // Reserved
	  vPortSVCHandler,                              // SVCall handler
      DebugMon_Handler,                         // Debug monitor handler
      0, // Reserved
	  xPortPendSVHandler, // The PendSV handler
	  xPortSysTickHandler, // The SysTick handler

	  /* -------------------     MAX32550 IP IRQ  ----------------------------------*/
	  PWF_IRQHandler, 		/* Power Fail */
      WDG_IRQHandler, 		/* Watchdog Timer */
	  USB_IRQHandler, 		/* USB */
      RTC_IRQHandler, 		/* RTC */
      TRNG_IRQHandler, 		/* True Random Number Generator */
      TIMER0_IRQHandler, 	/* Timer 0 */
	  TIMER1_IRQHandler, 	/* Timer 1 */
	  TIMER2_IRQHandler, 	/* Timer 2 */
	  TIMER3_IRQHandler, 	/* Timer 3 */
	  TIMER4_IRQHandler, 	/* Timer 4 */
	  TIMER5_IRQHandler, 	/* Timer 5 */
	  SC_IRQHandler, 		/* SmartCard 0 */
	  LCD_TFT_IRQHandler,	/* TFT LCD */
	  I2C_IRQHandler,		/* I2C 0	*/
      UART0_IRQHandler, 	/* UART 0 */
      UART1_IRQHandler, 	/* UART 1 */
      SPI0_IRQHandler, 		/* SPI 0 */
      SPI1_IRQHandler, 		/* SPI 1 */
      SPI2_IRQHandler, 		/* SPI 2 */
      SKBD_IRQHandler, 		/* Secure Keypad */
	  ADC_IRQHandler, 		/* ADC */
	  DAC_IRQHandler,		/* DAC */
      MSR_DSP_IRQHandler,	/* MSR DSP Done */
      FLC_IRQHandler, 		/* Flash Controlleur */
      GPIO0_IRQHandler, 	/* GPIO 0 */
      GPIO1_IRQHandler, 	/* GPIO 1 */
      GPIO2_IRQHandler, 	/* GPIO 2 */
	  CRYPTO_IRQHandler, 	/* Crypto Engine */
      DMA0_IRQHandler, 		/* DMA 0 */
      DMA1_IRQHandler, 		/* DMA 1 */
      DMA2_IRQHandler, 		/* DMA 2 */
      DMA3_IRQHandler, 		/* DMA 3 */
      0, //
      0, //
      0, //
      0, //
      0, //
      0, //
      0, //
      0, //
      0, //
      0, //
      0, //
      0, //
      0, //
      0, //
      0, //
      0, //
  };

// ----------------------------------------------------------------------------

// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.

void __attribute__ ((section(".after_vectors"))) Default_Handler(void)
{

#if defined(DEBUG)
  __DEBUG_BKPT();
#endif

  while (1);
}

// ----------------------------------------------------------------------------
