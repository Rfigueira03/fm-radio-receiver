/**
 * @file projeto.c
 * @brief Ponto de entrada (Entry Point) do Sistema de Rádio FM.
 * @author Rodrigo Figueira
 * @date 25/01/2024
 * * @details Este ficheiro contém a função main() que inicializa todos os
 * periféricos (I2C, LCD, RTC, Flash) e entrega o controlo à
 * Máquina de Estados (statemachine.c).
 */
#include "LPC17xx.h"
#include "statemachine.h"
#include "lcd.h"
#include "delay.h"
#include "keyboard.h"
#include "rtc_driver.h"
#include "I2C.h"

/**
 * @brief Função principal do programa.
 * @details Inicializa o hardware e entra no loop infinito da máquina de estados.
 * @return int Não retorna (loop infinito).
 */
int main(void)
{
    SystemInit();

    DELAY_Init();
    LCDText_Init();
    NAVBTN_Init();

    I2CMASTER_Init();
    I2CMASTER_SetFrequency(100000);


    ConfigurarTempoInicial();

    LCDText_Clear();
    LCDText_SetCursor(0,0);
    LCDText_WriteString("RDA5807 FM Radio");
    LCDText_SetCursor(1,0);
    LCDText_WriteString("A Iniciar...");
    DELAY_Milliseconds(1000);

    LCDText_Clear();
    CarregarDefinicoes();

    while (1){
    	switch (CURRENT_STATE){
            case NORMAL_STATE:
                normal_state_exec();
                break;

            case MAINTENANCE_STATE:
                maintenance_state_exec();
                break;

            case SETHOUR_STATE:
                sethour_state_exec();
                break;

            case SETDATE_STATE:
                setdate_state_exec();
                break;

            case DELETE_STATE:
            	delete_state_exec();
            	break;

            default:
                CURRENT_STATE = NORMAL_STATE;
                break;
        }
        DELAY_Milliseconds(100);
    }
    return 0;
}
