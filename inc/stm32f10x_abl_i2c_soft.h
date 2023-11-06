#ifndef __STM32F10x_ABL_I2C_SOFT_H
#define __STM32F10x_ABL_I2C_SOFT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

typedef struct
{
    uint32_t RCC_APB2Periph;
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_SCL_Pin;
    uint16_t GPIO_SDA_Pin;
} I2C_Soft_InitTypeDef;

void I2C_Soft_Init(
    I2C_Soft_InitTypeDef *I2CSoftx,
    uint32_t RCC_APB2Periph,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_SCL_Pin,
    uint16_t GPIO_SDA_Pin);

void I2C_Soft_Start(I2C_Soft_InitTypeDef *I2CSoftx);
void I2C_Soft_Stop(I2C_Soft_InitTypeDef *I2CSoftx);
void I2C_Soft_Wait(I2C_Soft_InitTypeDef *I2CSoftx);
void I2C_Soft_SendByte(I2C_Soft_InitTypeDef *I2CSoftx, uint8_t Byte);
uint8_t I2C_Soft_ReceiveByte(I2C_Soft_InitTypeDef *I2CSoftx);
void I2C_Soft_SendAck(I2C_Soft_InitTypeDef *I2CSoftx, uint8_t AckBit);
uint8_t I2C_Soft_ReceiveAck(I2C_Soft_InitTypeDef *I2CSoftx);

#ifdef __cplusplus
}
#endif

#endif
