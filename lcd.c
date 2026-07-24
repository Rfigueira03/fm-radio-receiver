/**
 * @file lcd.c
 * @brief Driver para LCD 16x2 em modo 4-bits.
 * @author Rodrigo PC
 */
#include "LPC17xx.h"
#include "delay.h"
#include "lcd.h"
#include <stdarg.h>
#include <stdio.h>

#define LCD_RS (1 << 3)
#define LCD_E  (1 << 2)
#define LCD_D4 (1 << 26)
#define LCD_D5 (1 << 25)
#define LCD_D6 (1 << 24)
#define LCD_D7 (1 << 23)

#define LCD_MASK (LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7)
#define LCD_PORT LPC_GPIO0

// Delays dos datasheet
#define CMD_DELAY_US   50
#define CLEAR_DELAY_MS 5
#define INIT_POWER_ON_MS 20

// funções locais
static inline void LCD_SetDataPins(uint8_t nibble);
static void LCD_WriteNibble(uint8_t nibble, bool rs);
static void LCD_PulseEnable(void);
static void LCD_WriteByte(uint8_t byte, bool rs);
static void LCD_Command(uint8_t cmd);
static void LCD_Data(uint8_t data);

/**
 * @brief Define os níveis lógicos dos pinos D4-D7 com base no nibble recebido.
 * @param nibble Os 4 bits menos significativos contêm os dados a colocar no barramento.
 */
static inline void LCD_SetDataPins(uint8_t nibble) {
    LCD_PORT->FIOCLR = LCD_MASK;

    if (nibble & 0x01) LCD_PORT->FIOSET = LCD_D4; // bit0 → D4
    if (nibble & 0x02) LCD_PORT->FIOSET = LCD_D5; // bit1 → D5
    if (nibble & 0x04) LCD_PORT->FIOSET = LCD_D6; // bit2 → D6
    if (nibble & 0x08) LCD_PORT->FIOSET = LCD_D7; // bit3 → D7
}

/**
 * @brief Gera um pulso no pino Enable (E) para o LCD processar os dados.
 * @note Respeita os tempos de setup e hold do controlador HD44780.
 */
static void LCD_PulseEnable(void) {
    LCD_PORT->FIOSET = LCD_E;
    DELAY_Microseconds(2);  // <- era 1 µs
    LCD_PORT->FIOCLR = LCD_E;
    DELAY_Microseconds(200); // <- era 50 µs
}

/**
 * @brief Envia meio byte (4 bits) para o LCD.
 * @param nibble Dados a enviar.
 * @param rs Estado do pino Register Select (true=Dados, false=Comando).
 */
static void LCD_WriteNibble(uint8_t nibble, bool rs) {
    if (rs) LCD_PORT->FIOSET = LCD_RS;
    else    LCD_PORT->FIOCLR = LCD_RS;

    LCD_SetDataPins(nibble & 0x0F);
    /* setup time antes de gerar pulso em E */
    DELAY_Microseconds(1);
    LCD_PulseEnable();
}

/**
 * @brief Envia um byte completo (8 bits) dividindo-o em dois nibbles.
 * @param byte O byte a enviar.
 * @param rs Estado do pino Register Select.
 */
static void LCD_WriteByte(uint8_t byte, bool rs) {
    /* enviar nibble alto depois nibble baixo */
    LCD_WriteNibble((byte >> 4) & 0x0F, rs);
    LCD_WriteNibble(byte & 0x0F, rs);
}

/**
 * @brief Envia um comando de controlo para o LCD (RS = 0).
 * @param cmd O código do comando.
 */
static void LCD_Command(uint8_t cmd) {
    LCD_WriteByte(cmd, false);
}

/**
 * @brief Envia um caracter/dado para a RAM do LCD (RS = 1).
 * @param data O caracter a escrever.
 */
static void LCD_Data(uint8_t data) {
    LCD_WriteByte(data, true);
}

//funções gerais
void LCDText_Init(void) {
    LCD_PORT->FIODIR |= (LCD_RS | LCD_E | LCD_MASK);

    DELAY_Milliseconds(20);     // >15 ms power on

    // 8-bit init sequence (enviada em nibbles altos)
    LCD_WriteNibble(0x03, false);
    DELAY_Milliseconds(5);      // >4.1 ms
    LCD_WriteNibble(0x03, false);
    DELAY_Microseconds(150);    // >100 µs
    LCD_WriteNibble(0x03, false);
    LCD_WriteNibble(0x02, false); // muda para 4-bit mode

    // comandos principais
    LCD_Command(0x28);          // 4-bit, 2 linhas, 5x8
    LCD_Command(0x0C);          // display ON, cursor OFF
    LCD_Command(0x06);          // auto-increment
    LCD_Command(0x01);          // clear display
    DELAY_Milliseconds(2);
}

void LCDText_WriteChar(char ch) {
    LCD_Data((uint8_t)ch);
}

void LCDText_WriteString(char *str) {
    while (str && *str) {
        LCDText_WriteChar(*str++);
    }
}

void LCDText_SetCursor(int row, int column) {
    uint8_t addr;
    if (row <= 0) addr = 0x00 + (uint8_t)column;
    else          addr = 0x40 + (uint8_t)column;
    LCD_Command(0x80 | (addr & 0x7F));
}

void LCDText_Clear(void) {
    LCD_Command(0x01);
    DELAY_Milliseconds(CLEAR_DELAY_MS);
}

void LCDText_Printf(char *fmt, ...) {
    char buf[64];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    LCDText_WriteString(buf);
}
