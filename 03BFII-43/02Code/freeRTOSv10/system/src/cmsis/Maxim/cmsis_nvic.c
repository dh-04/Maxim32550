#include <cmsis_nvic.h>

void __NVIC_SetVector(IRQn_Type IRQn, uint32_t vector) {
    uint32_t *vectors = (uint32_t *) SCB->VTOR;
    uint32_t i;

    /* Copy and switch to dynamic vectors if the first time called */
    if (SCB->VTOR != (unsigned int)NVIC_RAM_VECTOR_ADDRESS) {
        uint32_t *old_vectors = (uint32_t *) NVIC_FLASH_VECTOR_ADDRESS;
        vectors = (uint32_t *) NVIC_RAM_VECTOR_ADDRESS;
        for (i = 0; i < NVIC_NUM_VECTORS; i++) {
            vectors[i] = old_vectors[i];
        }
        SCB->VTOR = (uint32_t) NVIC_RAM_VECTOR_ADDRESS;
    }
    vectors[IRQn + NVIC_USER_IRQ_OFFSET] = vector;
}

uint32_t __NVIC_GetVector(IRQn_Type IRQn) {
    uint32_t *vectors = (uint32_t*)SCB->VTOR;
    return vectors[IRQn + NVIC_USER_IRQ_OFFSET];
}

unsigned int cpsie(void) {
	__enable_irq();
	return 0;
}
unsigned int cpsid(void) {
	__disable_irq();
	return 0;
}
