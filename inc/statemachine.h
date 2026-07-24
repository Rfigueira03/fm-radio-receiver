/**
 * @file statemachine.h
 * @brief Definição dos estados e protótipos da lógica principal.
 * @author Rodrigo PC
 */

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include "delay.h"
#include "I2C.h"
#include "lcd.h"
#include "keyboard.h"
#include "rtc_driver.h"
#include "flash.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

/**
 * @brief Enumeração dos Estados do Sistema.
 */
typedef enum {
    NORMAL_STATE,
    MAINTENANCE_STATE,
    SETDATE_STATE,
    SETHOUR_STATE,
	DELETE_STATE,
} SystemState_t;

/** @brief Variável global que guarda o estado atual */
extern SystemState_t CURRENT_STATE;

/**
 * @brief Envia as definições de frequência e volume para o rádio.
 */
void AtualizarRadio(void);

/**
 * @brief Verifica a Flash e carrega definições salvas ou padrão.
 */
void CarregarDefinicoes(void);

/**
 * @brief Configura o RTC com a hora de compilação/padrão.
 */
void ConfigurarTempoInicial(void);

/**
 * @brief Lógica do Estado NORMAL (Visualização e Controlo Volume/Freq).
 */
void normal_state_exec(void);

/**
 * @brief Lógica do Estado MANUTENÇÃO (Navegação no Menu).
 */
void maintenance_state_exec(void);

/**
 * @brief Lógica do Estado ACERTAR HORA.
 */
void sethour_state_exec(void);

/**
 * @brief Lógica do Estado ACERTAR DATA.
 */
void setdate_state_exec(void);

/**
 * @brief Lógica do Estado APAGAR DADOS (Gestão da Flash).
 */
void delete_state_exec(void);

#endif /* STATEMACHINE_H_ */
