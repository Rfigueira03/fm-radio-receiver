/**
 * @file keyboard.c
 * @brief Driver para leitura de teclado matricial 4x2.
 * @author Rodrigo PC
 */
#include "LPC17xx.h"
#include "keyboard.h"

#define COLUMN_PORT 2
#define COLUMN1_PIN (1 << 8) // P2.8 (CN5)
#define COLUMN2_PIN (1 << 7) // P2.7 (CN4)

#define ROW_PORT 2
#define ROW1_PIN (1 << 6) // P2.6 (CN3)
#define ROW2_PIN (1 << 5) // P2.5 (CN2)
#define ROW3_PIN (1 << 4) // P2.4 (CN1)
#define ROW4_PIN (1 << 3) // P2.3 (CN0)

#define COLUMN_MASK (COLUMN1_PIN | COLUMN2_PIN)
#define ROW_MASK (ROW1_PIN | ROW2_PIN | ROW3_PIN | ROW4_PIN)

static const NAVBTN_TypeDef Keymap[4][2] = {
	{ NAVBTN_NONE,  NAVBTN_UP     }, // row 1
	{ NAVBTN_BACK,  NAVBTN_DOWN   }, // row 2
	{ NAVBTN_CENTER, NAVBTN_LEFT   }, // row 3
	{ NAVBTN_ENTER, NAVBTN_RIGHT }  // row 4
};

static NAVBTN_TypeDef last_scan = NAVBTN_NONE;
static NAVBTN_TypeDef last_report = NAVBTN_NONE;

/**
 * @brief Lê o estado físico dos pinos das colunas.
 * @return uint32_t Valor lido do porto mascarado com os bits das colunas.
 */
static uint32_t read_columns(void) {
	return LPC_GPIO2->FIOPIN & COLUMN_MASK;
}

void NAVBTN_Init(void) {
	// Rows como saída (default = HIGH)
	LPC_GPIO2->FIODIR |= ROW_MASK;
	LPC_GPIO2->FIOSET = ROW_MASK;

	// Columns como entrada
	LPC_GPIO2->FIODIR &= ~COLUMN_MASK;

	last_scan = NAVBTN_NONE;
	last_report = NAVBTN_NONE;
}

NAVBTN_TypeDef NAVBTN_Read(void) {
	uint32_t col;
	NAVBTN_TypeDef found = NAVBTN_NONE;

	// garantir todas as linhas em HIGH
	LPC_GPIO2->FIOSET = ROW_MASK;

	for (int row = 0; row < 4; row++) {
		// coloca todas HIGH e desce só a linha atual
		LPC_GPIO2->FIOSET = ROW_MASK;
		switch (row) {
			case 0: LPC_GPIO2->FIOCLR = ROW1_PIN; break;
			case 1: LPC_GPIO2->FIOCLR = ROW2_PIN; break;
			case 2: LPC_GPIO2->FIOCLR = ROW3_PIN; break;
			case 3: LPC_GPIO2->FIOCLR = ROW4_PIN; break;
		}

		col = read_columns();

		if ((col & COLUMN1_PIN) == 0) {
			found = Keymap[row][0];
			goto done;
		}
		if ((col & COLUMN2_PIN) == 0) {
			found = Keymap[row][1];
			goto done;
		}
	}

done:
	LPC_GPIO2->FIOSET = ROW_MASK; // desativar tudo
	return found;
}

NAVBTN_TypeDef NAVBTN_Pressed(void) {
	NAVBTN_TypeDef nav = NAVBTN_Read();

	if (nav == NAVBTN_NONE) {
		last_scan = NAVBTN_NONE;
		return NAVBTN_NONE;
	}

	if (nav != last_scan) {
		last_scan = nav;
		return nav; // nova tecla
	}

	return NAVBTN_NONE;
}
