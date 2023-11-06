#ifndef __STM32F10x_ABL_LED_H
#define __STM32F10x_ABL_LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

typedef struct
{
    uint32_t RCC_APB2Periph;
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
} LED_InitTypeDef;

void LED_Init(
    LED_InitTypeDef *Ledx,
    uint32_t RCC_APB2Periph,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_Pin);

uint8_t LED_IsOn(LED_InitTypeDef *Ledx);
void LED_On(LED_InitTypeDef *Ledx);
void LED_Off(LED_InitTypeDef *Ledx);
void LED_Toggle(LED_InitTypeDef *Ledx);

void Led1_Init(LED_InitTypeDef *Ledx);
void Led2_Init(LED_InitTypeDef *Ledx);
void Led3_Init(LED_InitTypeDef *Ledx);

#ifdef __cplusplus
}
#endif

#endif
