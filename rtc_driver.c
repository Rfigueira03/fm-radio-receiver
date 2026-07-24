/**
 * @file rtc_driver.c
 * @brief Driver para controlo do Real Time Clock (RTC) do LPC1769.
 * @author Rodrigo PC
 */
#include "LPC17xx.h"  // Header principal do LPC1769
#include "time.h"     // Para os tipos time_t e struct tm
#include "rtc_driver.h"

/**
 * Faz a iniciação do RTC.
 * Segundos desde 00:00:00 UTC de 1 janeiro 1970 (Unix epoch).
 */
void RTC_Init(time_t seconds) {
    // Ligar a energia para o periférico RTC
    // (Bit 9 no registrador Power Control for Peripherals)
    LPC_SC->PCONP |= (1 << 9);

    // Parar o RTC (limpar bit CLKEN) para configuração
    LPC_RTC->CCR = 0;

    // Selecionar a fonte do clock:
    // Queremos o oscilador externo de 32.768 kHz
    // (Define o bit 4, CLKSRC = 1)
    LPC_RTC->CCR = (1 << 4);

    //Limpar registradores de interrupção (boa prática)
    LPC_RTC->ILR = 0;
    LPC_RTC->CIIR = 0;

    //Definir o tempo inicial usando a função auxiliar
    RTC_SetSeconds(seconds);

    //Iniciar o RTC (definir bit 0, CLKEN = 1)
    LPC_RTC->CCR |= (1 << 0);
}

/**
 * Devolve em dateTime o valor corrente do RTC.
 * dateTime: Ponteiro para a estrutura tm onde o tempo será armazenado.
 */
void RTC_GetTimeDate(struct tm *dateTime) {
    /*
     * Nota de Hardware: No LPC1769, a leitura de qualquer registrador 
     * de tempo (ex: SEC) "congela" (latches) o valor de todos os 
     * outros registradores de tempo, permitindo uma leitura atômica.
     */

    dateTime->tm_sec  = LPC_RTC->SEC;
    dateTime->tm_min  = LPC_RTC->MIN;
    dateTime->tm_hour = LPC_RTC->HOUR;
    dateTime->tm_mday = LPC_RTC->DOM;   // Dia do Mês (1-31)
    dateTime->tm_wday = LPC_RTC->DOW;   // Dia da Semana (0-6)
    dateTime->tm_yday = LPC_RTC->DOY;   // Dia do Ano (1-366)
    
    // Ajustes de offset:
    // RTC armazena Mês (1-12), struct tm usa (0-11)
    dateTime->tm_mon  = LPC_RTC->MONTH - 1;
    // RTC armazena Ano (ex: 2025), struct tm usa (anos desde 1900)
    dateTime->tm_year = LPC_RTC->YEAR - 1900;

    // Ajuste de Horário de Verão
    dateTime->tm_isdst = -1; 
}

/**
 * Realiza a atualização do RTC com os valores do parâmetro dateTime.
 * dateTime: Ponteiro com os novos valores de data e hora.
 */
void RTC_SetTimeDate(struct tm *dateTime) {
    //Armazenar o estado atual do Clock Control Register (CCR)
    uint32_t ccr_val = LPC_RTC->CCR;

    //Desabilitar o RTC (limpar CLKEN) para permitir a escrita
    LPC_RTC->CCR = ccr_val & ~(1 << 0);

    //Atualizar os registradores de tempo com os valores da struct
    LPC_RTC->SEC   = dateTime->tm_sec;
    LPC_RTC->MIN   = dateTime->tm_min;
    LPC_RTC->HOUR  = dateTime->tm_hour;
    LPC_RTC->DOM   = dateTime->tm_mday;
    LPC_RTC->DOW   = dateTime->tm_wday;
    LPC_RTC->DOY   = dateTime->tm_yday;

    //Ajustar os offsets antes de escrever
    LPC_RTC->MONTH = dateTime->tm_mon + 1;  // struct tm (0-11) -> RTC (1-12)
    LPC_RTC->YEAR  = dateTime->tm_year + 1900; // struct tm (desde 1900) -> RTC (ex: 2025)

    //Restaurar o estado original do CCR (que irá reabilitar o clock se estava ativo)
    LPC_RTC->CCR = ccr_val;
}

/**
 * Realiza a atualização do RTC com o valor de segundos (epoch time).
 * Segundos desde 00:00:00 UTC de 1 janeiro 1970.
 */
void RTC_SetSeconds(time_t seconds) {
    /*
     * O LPC1769 fornece o registrador CTIME0, que é exatamente
     * o contador de segundos desde a epoch. Escrever nele
     * atualiza automaticamente todos os outros registradores (SEC, MIN, etc.).
     */

	//struct tm *dateTime;
    
    // É recomendado desabilitar o RTC brevemente ao usar CTIME0
    uint32_t ccr_val = LPC_RTC->CCR;
    LPC_RTC->CCR = ccr_val & ~(1 << 0); // Limpar CLKEN
    
 //   LPC_RTC->CTIME0 = seconds;
    struct tm *dateTime = gmtime(&seconds);
    RTC_SetTimeDate(dateTime);
    LPC_RTC->CCR = ccr_val; // Restaurar
}

/**
 * Retorna o valor corrente do RTC, em segundos (epoch time).
 * time_t: Segundos desde 00:00:00 UTC de 1 janeiro 1970.
 */
time_t RTC_GetSeconds(void) {
    // O registrador CTIME0 é a representação atômica
    // do tempo em segundos (formato time_t).
    return LPC_RTC->CTIME0;
}
