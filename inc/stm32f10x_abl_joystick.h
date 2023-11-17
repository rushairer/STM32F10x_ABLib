#ifndef __STM32F10X_ABL_JOYSTICK_H
#define __STM32F10X_ABL_JOYSTICK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

typedef struct
{
    uint32_t RCC_APB2Periph_ADC;
    ADC_TypeDef *ADCx;
    uint32_t RCC_APB2Periph_PIN;
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_X_Pin;
    uint16_t GPIO_Y_Pin;
    uint8_t ADC_Channel_X;
    uint8_t ADC_Channel_Y;
    uint16_t GPIO_Key_Pin;
} JOYSTICK_InitTypeDef;

void JOYSTICK_Init(
    JOYSTICK_InitTypeDef *Joystickx,
    uint32_t RCC_APB2Periph_ADC,
    ADC_TypeDef *ADCx,
    uint32_t RCC_APB2Periph_PIN,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_X_Pin,
    uint16_t GPIO_Y_Pin,
    uint8_t ADC_Channel_X,
    uint8_t ADC_Channel_Y,
    uint16_t GPIO_Key_Pin);

uint16_t JOYSTICK_GetXValue(JOYSTICK_InitTypeDef *Joystickx);
uint16_t JOYSTICK_GetYValue(JOYSTICK_InitTypeDef *Joystickx);
uint8_t JOYSTICK_IsPressed(JOYSTICK_InitTypeDef *Joystickx);

#ifdef __cplusplus
}
#endif

#endif
