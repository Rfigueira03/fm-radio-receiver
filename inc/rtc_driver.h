/**
 * @file rtc_driver.h
 * @brief API para controlo do Real Time Clock.
 * @author Rodrigo PC
 */
#ifndef RTC_DRIVER_H
#define RTC_DRIVER_H

// Incluímos <time.h> para os tipos 'time_t' e 'struct tm'
#include <time.h>
/**
 * @brief Faz a iniciação do RTC.
 * @param seconds Segundos desde 00:00:00 UTC de 1 janeiro 1970.
 */
void RTC_Init(time_t seconds);

/**
 * @brief Devolve em dateTime o valor corrente do RTC.
 * @param dateTime Ponteiro para a estrutura tm onde o tempo será armazenado.
 */
void RTC_GetTimeDate(struct tm *dateTime);

/**
 * @brief Realiza a atualização do RTC com os valores do parâmetro dateTime.
 * @param dateTime Ponteiro com os novos valores de data e hora.
 */
void RTC_SetTimeDate(struct tm *dateTime);

/**
 * @brief Realiza a atualização do RTC com o valor em segundos.
 * @param seconds Segundos desde 00:00:00 UTC de 1 janeiro 1970.
 */
void RTC_SetSeconds(time_t seconds);

/**
 * @brief Obtém o tempo atual do RTC em formato Unix Timestamp.
 * @return time_t Segundos decorridos desde as 00:00:00 UTC de 1 de Janeiro de 1970 (Epoch).
 */
time_t RTC_GetSeconds(void);

#endif // RTC_DRIVER_H
