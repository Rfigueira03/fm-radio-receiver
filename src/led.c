/**
 * @file led.c
 * @brief Driver simples para controlo de um LED.
 * @author Rodrigo PC
 */
#include "LPC17xx.h"
#include "led.h"

#define LED_PIN (1 << 22) // LED1 = P0.22

static bool led_state = false;

void LED_Init(bool state) {
    LPC_GPIO0->FIODIR |= LED_PIN; // saída
    if (state) LED_On();
    else LED_Off();
}

bool LED_GetState(void) {
    return led_state;
}

void LED_On(void) {
    LPC_GPIO0->FIOSET = LED_PIN;
    led_state = true;
}

void LED_Off(void) {
    LPC_GPIO0->FIOCLR = LED_PIN;
    led_state = false;
}

void LED_Toggle(void) {
    if (led_state) LED_Off();
    else LED_On();
}
