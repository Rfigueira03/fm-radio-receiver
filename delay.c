/**
 * @file delay.c
 * @brief Funções de atraso (Delay) usando SysTick e Timer.
 * @author Rodrigo PC
 */
#include "LPC17xx.h"
#include "delay.h"

static volatile uint32_t msTicks = 0; // contador de milissegundos

void SysTick_Handler(void) {
    msTicks++;
}

int DELAY_Init(void) {
    SystemCoreClockUpdate();
    if (SysTick_Config(SystemCoreClock / 1000)) {
        return -1; // erro
    }
    // Configurar Timer2 para microsegundos
    LPC_SC->PCONP |= (1 << 22); // Power ON Timer2
    LPC_SC->PCLKSEL1 &= ~(3 << 12);
    LPC_TIM2->TCR = 0x02;
    LPC_TIM2->PR = (SystemCoreClock / 4 / 1000000) - 1; // 1 µs por incremento
    LPC_TIM2->TCR = 0x01; // Enable Timer2
    return 0;
}

void DELAY_Milliseconds(uint32_t millis) {
	uint32_t start = msTicks;
    while ((msTicks - start) < millis);
}

uint32_t DELAY_GetElapsedMillis(uint32_t start) {
    return (msTicks - start);
}

void DELAY_Microseconds(uint32_t waitUs) {
    LPC_TIM2->TC = 0;
    while (LPC_TIM2->TC < waitUs) __NOP();
}
