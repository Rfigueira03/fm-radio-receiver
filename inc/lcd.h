/**
 * @file lcd.h
 * @brief Driver para LCD 16x2 (Modo 4-bits).
 * @author Rodrigo PC
 */
#ifndef _LCD_H_
#define _LCD_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Inicializa o LCD, configura pinos e limpa o ecrã.
 */
void LCDText_Init(void);

/**
 * @brief Escreve um único caracter na posição atual do cursor.
 * @param ch Caracter a escrever.
 */
void LCDText_WriteChar(char ch);

/**
 * @brief Escreve uma string terminada em null.
 * @param str String a escrever.
 */
void LCDText_WriteString(char *str);

/**
 * @brief Move o cursor para uma posição específica.
 * @param row Linha (0 ou 1).
 * @param column Coluna (0 a 15).
 */
void LCDText_SetCursor(int row, int column);

/**
 * @brief Limpa todo o conteúdo do LCD e repõe o cursor na origem.
 */
void LCDText_Clear(void);

/**
 * @brief Escreve texto formatado no LCD (similar ao printf do C).
 * @param fmt String de formatação.
 * @param ... Argumentos variáveis.
 */
void LCDText_Printf(char *fmt, ...);

#endif
