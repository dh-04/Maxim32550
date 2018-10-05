#ifndef __SYSTEM_MAX32550_H
#define __SYSTEM_MAX32550_H

#ifdef __cplusplus
 extern "C" {
#endif 

typedef enum{
	MML_SYSTEM_FREQUENCY_108_MHZ		= 108000000,
	MML_SYSTEM_FREQUENCY_54_MHz			=  54000000,
	MML_SYSTEM_FREQUENCY_27_MHz			=  27000000,
	MML_SYSTEM_FREQUENCY_13_5_MHz		=  13500000,
	MML_SYSTEM_FREQUENCY_6_75_MHz 		=   6750000,
	MML_SYSTEM_FREQUENCY_3_375_MHz 		=   3375000,
	MML_SYSTEM_FREQUENCY_1_6875_MHz 	=   1687500,
	MML_SYSTEM_FREQUENCY_0_84375_MHz 	=    843750,
}mml_system_frequency;

/**
 * Backward compatibility with COABRA environement
 */
typedef enum {
	MML_INTC_PRIO_0,
	MML_INTC_PRIO_1,
	MML_INTC_PRIO_2,
	MML_INTC_PRIO_3,
	MML_INTC_PRIO_4,
	MML_INTC_PRIO_5,
	MML_INTC_PRIO_6,
	MML_INTC_PRIO_7,
} mml_intc_prio_t;

extern unsigned int priority_table[32];

extern unsigned int SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */

extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_MAX32550_H */

