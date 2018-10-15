#include <MAX325xx.h>

unsigned int SystemCoreClock;

/**
 * Table containing value of interrupt priority used in mml driver
 */
unsigned int priority_table[32] = {
	MML_INTC_PRIO_2, // Power Fail interrupt priority
	MML_INTC_PRIO_2, // Watchdog interrupt priority
	MML_INTC_PRIO_2, // USB interrupt priority
	MML_INTC_PRIO_2, // RTC interrupt priority
	MML_INTC_PRIO_2, // TRNG interrupt priority
	MML_INTC_PRIO_2, // TIMER 0 interrupt priority
	MML_INTC_PRIO_2, // TIMER 1 interrupt priority
	MML_INTC_PRIO_2, // TIMER 2 interrupt priority
	MML_INTC_PRIO_2, // TIMER 3 interrupt priority
	MML_INTC_PRIO_2, // TIMER 4 interrupt priority
	MML_INTC_PRIO_2, // TIMER 5 interrupt priority
	MML_INTC_PRIO_2, // Smart Card interrupt priority
	MML_INTC_PRIO_2, // Mono LCD/TFT interrupt priority
	MML_INTC_PRIO_5, // I2C interrupt priority
	MML_INTC_PRIO_3, // UART 0 interrupt priority
	MML_INTC_PRIO_3, // UART 1 interrupt priority
	MML_INTC_PRIO_3, // SPI 0 interrupt priority
	MML_INTC_PRIO_2, // SPI 1 interrupt priority
	MML_INTC_PRIO_5, // SPI 2 interrupt priority
	MML_INTC_PRIO_3, // Secure Keypad interrupt priority
	MML_INTC_PRIO_3, // ADC interrupt priority
	MML_INTC_PRIO_2, // DAC interrupt priority
	MML_INTC_PRIO_2, // MSR interrupt priority
	MML_INTC_PRIO_2, // Flash Controller interrupt priority
	MML_INTC_PRIO_2, // GPIO 0 interrupt priority
	MML_INTC_PRIO_2, // GPIO 1 interrupt priority
	MML_INTC_PRIO_2, // GPIO 2 interrupt priority
	MML_INTC_PRIO_2, // Crypto Engine interrupt priority
	MML_INTC_PRIO_5, // DMA 0 interrupt priority
	MML_INTC_PRIO_5, // DMA 1 interrupt priority
	MML_INTC_PRIO_5, // DMA 2 interrupt priority
	MML_INTC_PRIO_5, // DMA 3 interrupt priority
};

/**
  * @}
  */


static void SetSysClock(void);

/** @addtogroup MML_SYS
  * @{
  */

/**
  * @brief  Setup the microcontroller system.
  *         Initialize the Embedded Flash Interface, the PLL and update the 
  *         SystemCoreClock variable.
  * @param  None
  * @retval None
  */
void SystemInit (void)
{    

  /* Configure the System clock frequency, AHB/APBx prescalers and Flash settings */
  SetSysClock();
}

/**
  * @brief  Set System Core clock to the maximum frequency
  *
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate (void)
{
	mml_set_system_frequency(MML_SYSTEM_FREQUENCY_108_MHZ);
	mml_get_system_frequency(&SystemCoreClock);
}


static void SetSysClock(void)
{
	mml_set_system_frequency(MML_SYSTEM_FREQUENCY_108_MHZ);
}

#ifdef TARGET_UVISOR_SUPPORTED

extern uint32_t __uvisor_bss_main_start;
extern void check_acl(uint32_t* p_acl, uint32_t keyindex);

void SystemInitPre(void)
{
	/* Configure the Vector Table location add offset address ------------------*/
	SCB->VTOR = 0x20000000 | 0x00;
	  *(&__uvisor_bss_main_start)=0xCAFEFADE;
	  *(&__uvisor_bss_main_start+1)=(uint32_t)&check_acl;
}

void initialize_vector(void) {
	extern unsigned int __isr_vector_start;
	extern unsigned int __vectors_start;
	extern unsigned int __vectors_end;
	unsigned int *p_1 = &__vectors_start;
	unsigned int *p_2 = &__isr_vector_start;

	while (p_1 < &__vectors_end)
		*p_1++ = *p_2++;
}
#endif //TARGET_UVISOR_SUPPORTED
/**
  * @}
  */


/******************************************************************************/
/* EOF */
