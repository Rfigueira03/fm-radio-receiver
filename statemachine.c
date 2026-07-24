/**
 * @file statemachine.c
 * @brief Máquina de estados principal do Rádio FM.
 * @author Rodrigo PC
 * @date 25/11/2025
 */

#include "statemachine.h"

// Variáveis globais
char line1_str[17];
char line2_str[17];
float freq;
int volume;

SystemState_t CURRENT_STATE = NORMAL_STATE;

struct tm current_time;
struct tm edit_time;

#define MY_FLAG 0x12345678 //minha marca
#define SECTOR29_ADDR ((void*)0x00078000)
#define RDA_ADDR 0x10

// A Struct para organizar os dados
typedef struct {
    uint32_t flag_marca; //flag para comparar com a minha marca
    float    saved_freq;
    int      saved_vol;
} AppData_t;

#define RDA_SEQ_ADDR 0x10

void AtualizarRadio(void) {

	uint16_t channel = (uint16_t)((freq - 87.0f) / 0.1f);
    uint8_t data[8];


    //0xD2 -> DHIZ=1, DMUTE=1 (Som ON), BASS=1
    data[0] = 0xC0;
    //0x01 -> ENABLE=1
    data[1] = 0x01;

    //Parte superior do Canal
    data[2] = (channel >> 2);
    //Parte inferior + TUNE=1
    data[3] = ((channel & 0x03) << 6) | (1 << 4);

    data[4] = 0x00;
    data[5] = 0x00;

    // Byte 6 (Alto)
    data[6] = 0x80;
    // Byte 7 (Baixo): Volume (0 a 15)
    data[7] = 0x80 | (volume & 0x0F);

    int erro = I2CMASTER_Transmit(RDA_SEQ_ADDR, data, 8);

    if (erro != 0) {
        LCDText_SetCursor(1,0);
        LCDText_Printf("I2C Erro: %d       ", erro);
    }
}

/**
 * @brief Calcula o número máximo de dias de um mês.
 * @param mes Mês (0=Jan a 11=Dez).
 * @param ano_tm Ano no formato tm (anos desde 1900).
 * @return int Número de dias (28, 29, 30 ou 31).
 */
int ObterMaxDias(int mes, int ano_tm) {
    // Meses com 30 dias
    // Nota: tm_mon vai de 0 a 11
    if (mes == 3 || mes == 5 || mes == 8 || mes == 10) {
        return 30;
    }
    // Fevereiro
    if (mes == 1) {
        int ano_real = ano_tm + 1900;
        // Bissexto
        if ((ano_real % 4 == 0 && ano_real % 100 != 0) || (ano_real % 400 == 0)) {
            return 29;
        } else {
            return 28;
        }
    }
    return 31;
}

//RTC inicial
void ConfigurarTempoInicial(void) {
    struct tm initial_time;

    initial_time.tm_year = 2025 - 1900;
    initial_time.tm_mon  = 11 - 1;
    initial_time.tm_mday = 4;
    initial_time.tm_hour = 10;
    initial_time.tm_min  = 30;
    initial_time.tm_sec  = 0;
    initial_time.tm_isdst = -1;

    time_t initial_seconds = mktime(&initial_time);
    RTC_Init(initial_seconds);
}

//Carrega as predefinições ou os dados guardados
void CarregarDefinicoes(void) {
    AppData_t *flash_data = SECTOR29_ADDR; //ponteiro para o Setor 29

    if (flash_data->flag_marca == MY_FLAG) { //verificar marca
        freq = flash_data->saved_freq;
        volume = flash_data->saved_vol;
        LCDText_WriteString("Config Carregada");
    } else {
        freq = 93.5;
        volume = 10;
        LCDText_WriteString("Config Padrao");
    }
    AtualizarRadio();
    DELAY_Milliseconds(1000);
}

//Função de Guardar (botão C)
void GuardarDefinicoes(void) {

    LCDText_Clear();
    LCDText_WriteString("A Guardar...");

    if (FLASH_EraseSector(29) != 0) { //apagar antes de guardar
    	LCDText_Clear();
        LCDText_WriteString("Erro Erase!"); return;
    }

    static uint8_t page_buffer[256] __attribute__((aligned(4)));
    AppData_t *data_to_save = (AppData_t *)page_buffer;
    memset(page_buffer, 0, 256);

    data_to_save->flag_marca = MY_FLAG;
    data_to_save->saved_freq = freq;
    data_to_save->saved_vol  = volume;

   	LCDText_SetCursor(1,0);
   	unsigned int status = FLASH_WriteData((void*)SECTOR29_ADDR, page_buffer, 256);
    if(status == 0){
    	LCDText_WriteString("Dados guardado     ");
    } else {
    	char buf[32];
    	sprintf(buf, "ERRORguardar:%d    ", status);
    	LCDText_WriteString(buf);
    }
    DELAY_Milliseconds(1000);
}

void normal_state_exec(void) {
	char line1[32];
	char line2[32];

    NAVBTN_TypeDef Key = NAVBTN_NONE;

   	LCDText_SetCursor(0,0);
   	RTC_GetTimeDate(&current_time);
   	sprintf(line1, "%02d/%02d/%04d V:%d   ",
			current_time.tm_mday,
			current_time.tm_mon + 1,
			current_time.tm_year + 1900, volume);
    LCDText_WriteString(line1);

    LCDText_SetCursor(1,0);
    sprintf(line2, "%02d:%02d:%02d %d.%dMHz",
    		current_time.tm_hour,
			current_time.tm_min,
			current_time.tm_sec,
			(int)freq,
			(int)((freq - (int)freq) * 10));
    LCDText_WriteString(line2);

   	Key = NAVBTN_Pressed();
   		if (Key != NAVBTN_NONE) {
   			bool change = false; //flag para atualizar o radio
   			switch (Key) {
    			case NAVBTN_UP:
    				if (volume < 15){
    					volume++;
    					change = true;
    				}
    				break;
    			case NAVBTN_DOWN:
    				if (volume > 0){
    					volume--;
    				}
    				change = true;
    				break;
    			case NAVBTN_LEFT:
    				if (freq > 87){
    					freq -= 0.1;
    				}
    				change = true;
    				break;
    			case NAVBTN_RIGHT:
    				if (freq < 108){
    					freq += 0.1;
    				}
    				change = true;
    				break;
    			case NAVBTN_CENTER:
    				GuardarDefinicoes();
    				break;
    			case NAVBTN_ENTER:
    				CURRENT_STATE = MAINTENANCE_STATE;
    				break;
    			default: break;
    	}
   		if (change) AtualizarRadio();
    }
}

void maintenance_state_exec(void) {
    static int menu_cursor = 0;
    NAVBTN_TypeDef Key = NAVBTN_Pressed();

    if (Key != NAVBTN_NONE) {
        switch (Key) {
            case NAVBTN_UP:
                menu_cursor--;
                if (menu_cursor < 0) menu_cursor = 3;
                break;
            case NAVBTN_DOWN:
                menu_cursor++;
                if (menu_cursor > 3) menu_cursor = 0;
                break;
            case NAVBTN_ENTER:
                if (menu_cursor == 0) {
                    RTC_GetTimeDate(&edit_time);
                    CURRENT_STATE = SETHOUR_STATE;
                }
                else if (menu_cursor == 1) {
                    RTC_GetTimeDate(&edit_time);
                    CURRENT_STATE = SETDATE_STATE;
                }
                else if (menu_cursor == 2) {
                	CURRENT_STATE = DELETE_STATE;
                }
                else {
                    CURRENT_STATE = NORMAL_STATE;
                }
            default: break;
        }
    }

    LCDText_SetCursor(0,0);
    LCDText_WriteString("Menu Manutencao ");

    LCDText_SetCursor(1,0);
    if (menu_cursor == 0) LCDText_WriteString("> Acertar Hora  ");
    else if (menu_cursor == 1) LCDText_WriteString("> Acertar Data  ");
    else if (menu_cursor == 2) LCDText_WriteString("> Apagar Dados  ");
    else LCDText_WriteString("> Voltar          ");
}

void sethour_state_exec(void) {
    static int edit_field = 0;
    NAVBTN_TypeDef Key = NAVBTN_Pressed();

    if (Key != NAVBTN_NONE) {
        switch (Key) {
            case NAVBTN_UP:
            	if (edit_field == 0) {
            		edit_time.tm_hour++;
            		if (edit_time.tm_hour > 23) edit_time.tm_hour = 0;
            	} else if (edit_field == 1){
            		edit_time.tm_min++;
            		if (edit_time.tm_min > 59) edit_time.tm_min = 0;
            	} else{
            		edit_time.tm_sec++;
            		if (edit_time.tm_sec > 59) edit_time.tm_sec = 0;
            	}
                break;

            case NAVBTN_DOWN:
            	if (edit_field == 0) {
            		edit_time.tm_hour--;
            		if (edit_time.tm_hour < 0) edit_time.tm_hour = 23;
            	} else if (edit_field == 1) {
            		edit_time.tm_min--;
            		if (edit_time.tm_min < 0) edit_time.tm_min = 59;
            	} else {
            		edit_time.tm_sec--;
            		if (edit_time.tm_sec < 0) edit_time.tm_sec = 59;
            	}
                break;

            case NAVBTN_ENTER:
            	edit_field++; // Passar para o próximo campo
            	if (edit_field > 2) {
            		RTC_SetTimeDate(&edit_time); // Gravar no RTC
            		edit_field = 0; // Reset para a próxima vez
            		CURRENT_STATE = MAINTENANCE_STATE;
            		LCDText_Clear();
            		return;
            	}
                break;
            default: break;
        }
    }

    LCDText_SetCursor(0,0);
    LCDText_WriteString("  Acertar Hora  ");

    //Horas Centradas
    sprintf(line2_str, "    %02d:%02d:%02d    ",
    		edit_time.tm_hour,
			edit_time.tm_min,
			edit_time.tm_sec);

    LCDText_SetCursor(1,0);
    LCDText_WriteString(line2_str);

    if (edit_field == 0)      LCDText_SetCursor(1, 4); // Em cima das Horas
    else if (edit_field == 1) LCDText_SetCursor(1, 7); // Em cima dos Minutos
    else                      LCDText_SetCursor(1, 10); // Em cima dos Segundos
}

void setdate_state_exec(void) {
    static int edit_field = 0;
    NAVBTN_TypeDef Key = NAVBTN_Pressed();
    int max_dias = ObterMaxDias(edit_time.tm_mon, edit_time.tm_year); //para ver o máx de dias no mês

    if (Key != NAVBTN_NONE) {
    	switch (Key) {
    		case NAVBTN_UP:
    			if (edit_field == 0) {      // Dia
    				edit_time.tm_mday++;
    				if (edit_time.tm_mday > max_dias) edit_time.tm_mday = 1;
    			} else if (edit_field == 1) { // Mês
    				edit_time.tm_mon++;
    				if (edit_time.tm_mon > 11) edit_time.tm_mon = 0;
    				//corrigir dia se o mês selecionado não tiver esse nº de dia
    				int novo_max = ObterMaxDias(edit_time.tm_mon, edit_time.tm_year);
    				if (edit_time.tm_mday > novo_max) edit_time.tm_mday = novo_max;
    			} else {                    // Ano
    				edit_time.tm_year++;
    				//corrigir anos Bissextos
    				int novo_max = ObterMaxDias(edit_time.tm_mon, edit_time.tm_year);
    				if (edit_time.tm_mday > novo_max) edit_time.tm_mday = novo_max;
    			}
    			break;

    		case NAVBTN_DOWN:
    			if (edit_field == 0) {      // Dia
    				edit_time.tm_mday--;
    				if (edit_time.tm_mday < 1) edit_time.tm_mday = max_dias;
    			} else if (edit_field == 1) { // Mês
    				edit_time.tm_mon--;
    				if (edit_time.tm_mon < 0) edit_time.tm_mon = 11;
    				// correção
    				int novo_max = ObterMaxDias(edit_time.tm_mon, edit_time.tm_year);
    				if (edit_time.tm_mday > novo_max) edit_time.tm_mday = novo_max;
    			} else {                    // Ano
    				edit_time.tm_year--;
    				int novo_max = ObterMaxDias(edit_time.tm_mon, edit_time.tm_year);
    				if (edit_time.tm_mday > novo_max) edit_time.tm_mday = novo_max;
    			}
                    break;

    		case NAVBTN_ENTER:
    			edit_field++;
    			if (edit_field > 2) {
    				RTC_SetTimeDate(&edit_time);
    				edit_field = 0;
    				CURRENT_STATE = MAINTENANCE_STATE;
    				LCDText_Clear();
    				return;
    			}
    			break;
    		default: break;
    	}
    }

    LCDText_SetCursor(0,0);
    LCDText_WriteString("  Acertar Data  ");

    sprintf(line2_str, "   %02d/%02d/%04d   ",
    		edit_time.tm_mday,
			edit_time.tm_mon + 1,
			edit_time.tm_year + 1900);

    LCDText_SetCursor(1,0);
    LCDText_WriteString(line2_str);

    if (edit_field == 0)      LCDText_SetCursor(1, 3); // Dia
    else if (edit_field == 1) LCDText_SetCursor(1, 6); // Mês
    else                      LCDText_SetCursor(1, 9); // Ano
}

void delete_state_exec(void) {
    char line1[32];
    AppData_t *flash_data = (AppData_t*)SECTOR29_ADDR;
    NAVBTN_TypeDef Key = NAVBTN_Pressed();

    LCDText_SetCursor(0,0);

    if (flash_data->flag_marca == MY_FLAG){
        sprintf(line1, "V:%02d F:%02d.%dMHz  ",
                flash_data->saved_vol,
                (int)flash_data->saved_freq,
                (int)((flash_data->saved_freq - (int)flash_data->saved_freq) * 10));
    } else {
        sprintf(line1, "V:%02d F:%03d.%dMHz  ",
                volume,
                (int)freq,
                (int)((freq - (int)freq) * 10));
    }
    LCDText_WriteString(line1);

    LCDText_SetCursor(1,0);
    LCDText_WriteString("B-Back E-Delete ");

    if (Key != NAVBTN_NONE){
        switch (Key){
            case NAVBTN_ENTER:
                LCDText_Clear();
                LCDText_SetCursor(0,0);
                LCDText_WriteString("A remover...");
                int erro = FLASH_EraseSector(29);

                LCDText_SetCursor(1,0);
                if (erro != 0){
                    LCDText_WriteString("Erro Erase!");
                } else {
                    LCDText_WriteString("Dados removidos!");
                }

                DELAY_Milliseconds(1500);
                LCDText_Clear();
                CURRENT_STATE = MAINTENANCE_STATE;
                break;

            case NAVBTN_BACK:
                LCDText_Clear();
                CURRENT_STATE = MAINTENANCE_STATE;
                break;

            default: break;
        }
    }
}
