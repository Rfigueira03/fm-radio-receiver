/**
 * @file I2C.h
 * @brief Driver I2C Master para LPC1769.
 * @author Rodrigo PC
 * @date 24/11/2025
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdbool.h>
/**
 * @brief Faz a iniciação do controlador e configura os respetivos pinos I2C0.
 */
void I2CMASTER_Init(void);

/**
 * @brief Configura o ritmo de envio/receção (Baudrate).
 * @param frequency Frequência em Hz (ex: 100000 para 100kHz).
 */
void I2CMASTER_SetFrequency(int frequency);

/**
 * @brief Realiza a transmissão dos dados para um dispositivo escravo.
 * @param devAddress Endereço do dispositivo (7-bit).
 * @param data Ponteiro para o buffer de dados a enviar.
 * @param size Número de bytes a enviar.
 * @return int 0 em caso de sucesso, valor negativo em caso de erro.
 */
int I2CMASTER_Transmit(unsigned char devAddress, void *data, unsigned int size);

/**
 * @brief Realiza a receção de dados de um dispositivo I2C.
 * @param devAddress Endereço do dispositivo escravo (7-bit).
 * @param data Ponteiro para o buffer onde os dados lidos serão guardados.
 * @param size Número máximo de bytes a receber.
 * @return int O número de bytes recebidos com sucesso ou um código de erro negativo.
 */
int I2CMASTER_Receive(unsigned char devAddress, void *data, unsigned int size);

#endif /* I2C_H_ */
