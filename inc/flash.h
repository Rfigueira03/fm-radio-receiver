/**
 * @file flash.h
 * @brief Driver para gestão da memória Flash interna (IAP).
 * @author Rodrigo PC
 */

#ifndef FLASH_H_
#define FLASH_H_

#include <stdint.h>
/**
 * @brief Apaga o conteúdo de um setor da Flash.
 * @note Esta função desliga interrupções temporariamente se chamada via wrapper seguro.
 * @param sector Número do setor a apagar (ex: 29).
 * @return unsigned int Código de erro IAP (0 = Sucesso).
 */
unsigned int FLASH_EraseSector(unsigned int sector);

/**
 * @brief Escreve um bloco de dados na Flash.
 * @param dstAddr Endereço de destino na Flash.
 * @param srcAddr Endereço de origem na RAM (deve estar alinhado a 4 bytes).
 * @param size Dimensão em bytes (256, 512, 1024 ou 4096).
 * @return unsigned int Código de erro IAP (0 = Sucesso).
 */
unsigned int FLASH_WriteData(void *dstAddr, void *srcAddr, unsigned int size);

/**
 * @brief Verifica se a gravação na Flash foi bem sucedida comparando blocos de memória.
 * @note Utiliza o comando IAP "Compare" (56).
 * @param dstAddr Endereço do bloco de destino na Flash.
 * @param srcAddr Endereço do bloco de origem (RAM ou Flash).
 * @param size Dimensão da comparação em bytes (deve ser múltiplo de 4).
 * @return unsigned int Retorna 0 (CMD_SUCCESS) se os conteúdos forem iguais, ou código de erro se diferentes.
 */
unsigned int FLASH_VerifyData(void *dstAddr, void *srcAddr, unsigned int size);

#endif
