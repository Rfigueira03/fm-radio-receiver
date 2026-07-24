/**
 * @file delay.h
 * @brief Funções de atraso temporal (Blocking Delays).
 * @author Rodrigo PC
 */
#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>

/**
 * @brief Inicializa o SysTick e Timer para contagem de tempo.
 * @return int 0 se sucesso, -1 se erro.
 */
int DELAY_Init(void);

/**
 * @brief Gera um atraso em milissegundos (bloqueante).
 * @param millis Número de milissegundos a esperar.
 */
void DELAY_Milliseconds(uint32_t millis);

/**
 * @brief Gera um atraso em microssegundos (bloqueante).
 * @param waitUs Número de microssegundos a esperar.
 */
void DELAY_Microseconds(uint32_t waitUs);

/**
 * @brief Calcula o tempo decorrido em milissegundos desde um instante inicial.
 * @note Útil para medir intervalos de tempo não bloqueantes.
 * @param start O valor do contador de ticks (ms) no início da contagem.
 * @return uint32_t O número de milissegundos passados desde 'start'.
 */
uint32_t DELAY_GetElapsedMillis(uint32_t start);

#endif
