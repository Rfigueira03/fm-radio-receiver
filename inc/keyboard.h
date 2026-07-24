/**
 * @file keyboard.h
 * @brief Driver para leitura do Teclado Matricial.
 * @author Rodrigo PC
 */
#ifndef KEYBOARD_H_
#define KEYBOARD_H_

/**
 * @brief Enumeração das teclas de navegação disponíveis.
 */
typedef enum {
	NAVBTN_NONE = -1,
	NAVBTN_UP = 1,
	NAVBTN_DOWN,
	NAVBTN_LEFT,
	NAVBTN_RIGHT,
	NAVBTN_CENTER,
	NAVBTN_BACK,
	NAVBTN_ENTER
} NAVBTN_TypeDef;

/**
 * @brief Inicializa os pinos do teclado (Linhas como Saída, Colunas como Entrada).
 */
void NAVBTN_Init(void);

/**
 * @brief Lê o estado atual do teclado (Varrimento simples).
 * @return NAVBTN_TypeDef Tecla detetada no momento da leitura.
 */
NAVBTN_TypeDef NAVBTN_Read(void);

/**
 * @brief Verifica se houve uma nova pressão de tecla (com lógica de estado anterior).
 * @return NAVBTN_TypeDef Tecla pressionada ou NAVBTN_NONE.
 */
NAVBTN_TypeDef NAVBTN_Pressed(void);

#endif
