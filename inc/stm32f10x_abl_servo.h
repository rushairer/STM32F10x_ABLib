#ifndef __STM32F10X_ABL_SERVO_H
#define __STM32F10X_ABL_SERVO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

typedef struct {
    uint32_t RCC_APBxPeriph_TIM;
    TIM_TypeDef *TIMx;
    uint8_t CH;

    uint32_t RCC_APB2Periph_PIN;
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
} SERVO_TypeDef;

void SERVO_Init(
    SERVO_TypeDef *Servox,
    uint32_t RCC_APBxPeriph_TIM,
    TIM_TypeDef *TIMx,
    uint8_t CH,
    uint32_t RCC_APB2Periph_PIN,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_Pin);

void SERVO_SetAngle(SERVO_TypeDef *Servox, float Angle);

#ifdef __cplusplus
}
#endif

#endif
