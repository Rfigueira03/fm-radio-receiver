/**
 * @file flash.c
 * @brief Driver para escrita e apagamento da memória Flash interna (IAP).
 * @author Rodrigo PC
 */
#include "LPC17xx.h"

#include "flash.h"

#define IAP_ADDR 0x1FFF1FF1

typedef void (*IAP)(unsigned int [], unsigned int []);
static const IAP iap_entry = (IAP) IAP_ADDR;

/**
 * @brief Converte um endereço de memória físico no número do setor Flash correspondente.
 * @param addr Endereço de memória (ex: 0x00078000).
 * @return unsigned int O número do setor (0 a 29).
 */
static unsigned int GetSectorNumber(uint32_t addr)
{
    // Zona 1: Primeiros 64KB (Setores 0 a 15 de 4KB)
    if (addr < 0x00010000)
    {
        return addr / 4096; // 4096 = 4kB
    }
    // Zona 2: Restante memória (Setores 16 a 29 de 32KB)
    else
    {
        // Subtraímos os 64KB e dividimos por 32KB
        return 16 + ((addr - 0x00010000) / 32768);
    }
}

unsigned int FLASH_EraseSector(unsigned int sector){
	unsigned int cmd[5], res[4];

	//preparar para escrever
	cmd[0] = 50;
	cmd[1] = sector;
    cmd[2] = sector;
    iap_entry(cmd, res);
    if (res[0] != 0) return res[0];

    // apagar sector
    cmd[0] = 52;
    cmd[1] = sector;
    cmd[2] = sector;
    cmd[3] = SystemCoreClock / 1000;   //Dividir o clck por 1000 para dar khz
    iap_entry(cmd, res);

    return res[0];
}

unsigned int FLASH_WriteData(void *dstAddr, void *srcAddr, unsigned int size){
	unsigned int cmd[5], res[4];
	if(size != 256 && size != 512) return 7;

	unsigned int sector = GetSectorNumber((uint32_t)dstAddr);
	//Preparar para escrever
	cmd[0] = 50;
	cmd[1] = sector;
	cmd[2] = sector;
	iap_entry(cmd, res);
	if (res[0] != 0) return res[0];

	//Copiar RAM para flash
	cmd[0] = 51;
	cmd[1] = (unsigned int)dstAddr;
	cmd[2] = (unsigned int)srcAddr;
	cmd[3] = size;
	cmd[4] = SystemCoreClock/1000;
	iap_entry(cmd,res);

	return res[0];
}

unsigned int FLASH_VerifyData(void *dstAddr, void *srcAddr, unsigned int size){
	if(size % 4 != 0) return 1; //COMANDO INVALIDO
	unsigned int cmd[5], res[4];

	//Comparação
	cmd[0] = 56;
	cmd[1] = (unsigned int)dstAddr;
	cmd[2] = (unsigned int)srcAddr;
	cmd[3] = size;
	iap_entry(cmd,res);

	return res[0];
}
