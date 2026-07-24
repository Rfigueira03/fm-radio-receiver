/**
 * @file led.h
 * @brief Driver de controlo de LED.
 * @author Rodrigo PC
 */
#ifndef LED_H
#define LED_H

#include <stdbool.h>

/**
 * @brief Inicializa o pino do LED.
 * @param state Estado inicial.
 */
void LED_Init(bool state);

/**
 * @brief Obtém o estado atual do LED.
 * @return true se ligado, false se desligado.
 */
bool LED_GetState(void);

/** @brief Liga o LED. */
void LED_On(void);

/** @brief Desliga o LED. */
void LED_Off(void);

/** @brief Inverte o estado do LED. */
void LED_Toggle(void);

#endif
