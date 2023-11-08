#ifndef __STM32F10x_ABL_KEY4X4_H
#define __STM32F10x_ABL_KEY4X4_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

typedef struct
{
    uint32_t RCC_APB2Periph_R;
    GPIO_TypeDef *GPIOx_R;
    uint16_t GPIO_Pin_R1;
    uint16_t GPIO_Pin_R2;
    uint16_t GPIO_Pin_R3;
    uint16_t GPIO_Pin_R4;
    uint32_t RCC_APB2Periph_C;
    GPIO_TypeDef *GPIOx_C;
    uint16_t GPIO_Pin_C1;
    uint16_t GPIO_Pin_C2;
    uint16_t GPIO_Pin_C3;
    uint16_t GPIO_Pin_C4;
} KEY4X4_InitTypeDef;

void KEY4X4_Init(
    KEY4X4_InitTypeDef *Key4x4,
    uint32_t RCC_APB2Periph_R,
    GPIO_TypeDef *GPIOx_R,
    uint16_t GPIO_Pin_R1,
    uint16_t GPIO_Pin_R2,
    uint16_t GPIO_Pin_R3,
    uint16_t GPIO_Pin_R4,
    uint32_t RCC_APB2Periph_C,
    GPIO_TypeDef *GPIOx_C,
    uint16_t GPIO_Pin_C1,
    uint16_t GPIO_Pin_C2,
    uint16_t GPIO_Pin_C3,
    uint16_t GPIO_Pin_C4);

uint16_t KEY4X4_GetKey(KEY4X4_InitTypeDef *Key4x4);

#ifdef __cplusplus
}
#endif

#endif
