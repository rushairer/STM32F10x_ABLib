#ifndef __STM32F10X_ABL_KEY_H
#define __STM32F10X_ABL_KEY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

typedef struct
{
    uint32_t RCC_APB2Periph;
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
} KEY_InitTypeDef;

void KEY_Init(
    KEY_InitTypeDef *Keyx,
    uint32_t RCC_APB2Periph,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_Pin);

uint8_t KEY_IsPressed(KEY_InitTypeDef *Keyx);

#ifdef __cplusplus
}
#endif

#endif
