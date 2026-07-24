/**
 * @file I2C.c
 * @brief Driver I2C em modo Master (Polling).
 * @author Rodrigo PC
 * @date 24/11/2025
 */
#include "LPC17xx.h"
#include "I2C.h"

#define I2C0_ENABLE  (1 << 6)
#define I2C0_START   (1 << 5)
#define I2C0_STOP    (1 << 4)
#define I2C0_SI      (1 << 3)
#define I2C0_AA      (1 << 2)

/**
 * @brief Espera (bloqueante) até que a flag de estado I2C (SI) seja ativada.
 * @note Implementa um mecanismo de timeout para evitar bloqueio infinito.
 * @return int 0 se sucesso, -1 se ocorrer timeout.
 */
static int I2C_WaitSI(void)
{
    volatile int timeout = 2000000;

    while (!(LPC_I2C0->I2CONSET & I2C0_SI))
    {
        timeout--;
        if (timeout <= 0) return -1; // Timeout real
    }
    return 0;
}

void I2CMASTER_Init(void)
{
    //Ligar Energia I2C0
    LPC_SC->PCONP |= (1 << 7);

    //Configurar P0.27 (SDA) e P0.28 (SCL)
    LPC_PINCON->PINSEL1 &= ~((3<<22) | (3<<24));
    LPC_PINCON->PINSEL1 |=  ((1<<22) | (1<<24));

    //Limpar Flags
    LPC_I2C0->I2CONCLR = I2C0_ENABLE | I2C0_START | I2C0_SI | I2C0_AA | I2C0_STOP;

    I2CMASTER_SetFrequency(100000);

    LPC_I2C0->I2CONSET = I2C0_ENABLE;
}

void I2CMASTER_SetFrequency(int frequency)
{
    uint32_t div = SystemCoreClock / frequency;
    LPC_I2C0->I2SCLH = div / 2;
    LPC_I2C0->I2SCLL = div / 2;
}

int I2CMASTER_Transmit(unsigned char addr, void *data, unsigned int size)
{
    unsigned char *p = (unsigned char*)data;

    //START Condition
    LPC_I2C0->I2CONCLR = I2C0_START | I2C0_STOP | I2C0_SI | I2C0_AA;
    LPC_I2C0->I2CONSET = I2C0_START;

    if (I2C_WaitSI() < 0) { LPC_I2C0->I2CONSET = I2C0_STOP; return -10; }
    if ((LPC_I2C0->I2STAT & 0xF8) != 0x08) {
        LPC_I2C0->I2CONSET = I2C0_STOP; LPC_I2C0->I2CONCLR = I2C0_START | I2C0_SI;
        return -11;
    }

    //Enviar Endereço + Write
    LPC_I2C0->I2DAT = (addr << 1) | 0;
    LPC_I2C0->I2CONCLR = I2C0_START | I2C0_SI;

    if (I2C_WaitSI() < 0) return -12;
    if (LPC_I2C0->I2STAT != 0x18) {
        LPC_I2C0->I2CONSET = I2C0_STOP; LPC_I2C0->I2CONCLR = I2C0_SI;
        return -13; // NACK no Endereço
    }

    //Enviar Dados
    for (unsigned int i = 0; i < size; i++)
    {
        LPC_I2C0->I2DAT = p[i];
        LPC_I2C0->I2CONCLR = I2C0_SI;

        if (I2C_WaitSI() < 0) return -14;

        if (LPC_I2C0->I2STAT != 0x28) {
            // Enviar STOP antes de sair para libertar o bus
            LPC_I2C0->I2CONSET = I2C0_STOP;
            LPC_I2C0->I2CONCLR = I2C0_SI;
            // Retorna o byte de erro
            return -150 - i;
        }
    }

    //STOP Condition
    LPC_I2C0->I2CONSET = I2C0_STOP;
    LPC_I2C0->I2CONCLR = I2C0_SI;

    int stop_wait = 100000;
    while((LPC_I2C0->I2CONSET & I2C0_STOP) && (--stop_wait > 0));

    return 0;
}

int I2CMASTER_Receive(unsigned char addr, void *data, unsigned int size)
{
    unsigned char *p = (unsigned char*)data;

    //START
    LPC_I2C0->I2CONCLR = I2C0_START | I2C0_STOP | I2C0_SI | I2C0_AA;
    LPC_I2C0->I2CONSET = I2C0_START;

    if (I2C_WaitSI() < 0) { LPC_I2C0->I2CONSET = I2C0_STOP; return -20; }
    if ((LPC_I2C0->I2STAT & 0xF8) != 0x08) {
        LPC_I2C0->I2CONSET = I2C0_STOP; LPC_I2C0->I2CONCLR = I2C0_START | I2C0_SI;
        return -21;
    }

    //ENDEREÇO + READ (Bit 0 = 1)
    LPC_I2C0->I2DAT = (addr << 1) | 1;
    LPC_I2C0->I2CONCLR = I2C0_START | I2C0_SI; // Limpa flags para enviar

    if (I2C_WaitSI() < 0) return -22;
    if (LPC_I2C0->I2STAT != 0x40) { // 0x40 = SLA+R enviado, ACK recebido
        LPC_I2C0->I2CONSET = I2C0_STOP; LPC_I2C0->I2CONCLR = I2C0_SI;
        return -23;
    }

    //LER DADOS
    for (unsigned int i = 0; i < size; i++) {
        if (i == size - 1)
            LPC_I2C0->I2CONCLR = I2C0_AA;
        else
            LPC_I2C0->I2CONSET = I2C0_AA;

        LPC_I2C0->I2CONCLR = I2C0_SI;

        if (I2C_WaitSI() < 0) return -24;

        p[i] = LPC_I2C0->I2DAT; // Guarda o dado
    }

    //STOP
    LPC_I2C0->I2CONSET = I2C0_STOP;
    LPC_I2C0->I2CONCLR = I2C0_SI;

    volatile int stop_wait = 100000;
    while((LPC_I2C0->I2CONSET & I2C0_STOP) && (--stop_wait > 0));

    return size;
}
