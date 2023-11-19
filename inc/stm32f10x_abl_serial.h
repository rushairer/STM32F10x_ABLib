#ifndef __STM32F10X_ABL_SERIAL_H
#define __STM32F10X_ABL_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

typedef struct {
    uint32_t RCC_APBxPeriph_USART;
    USART_TypeDef *USARTx;

    uint32_t RCC_APB2Periph_PIN;
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Tx_Pin;
    uint16_t GPIO_Rx_Pin;

    uint32_t BaudRate;

    uint8_t RxData[128];
    uint8_t RxFlag;

} SERIAL_InitTypeDef;

void SERIAL_Init(
    SERIAL_InitTypeDef *Serialx,
    uint32_t RCC_APBxPeriph_USART,
    USART_TypeDef *USARTx,
    uint32_t RCC_APB2Periph_PIN,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_Tx_Pin,
    uint16_t GPIO_Rx_Pin,
    uint32_t BaudRate);

void SERIAL_SendByte(SERIAL_InitTypeDef *Serialx, uint8_t Byte);
uint32_t SERIAL_ReceiveByte(SERIAL_InitTypeDef *Serialx, uint8_t *Byte);
void SERIAL_SendArray(SERIAL_InitTypeDef *Serialx, uint8_t *Array, uint16_t Length);
void SERIAL_SendString(SERIAL_InitTypeDef *Serialx, char *String);
void SERIAL_SendNumber(SERIAL_InitTypeDef *Serialx, uint32_t Number, uint8_t Length);
void SERIAL_SendData(SERIAL_InitTypeDef *Serialx, uint8_t *Array, uint16_t Length);
void SERIAL_Printf(SERIAL_InitTypeDef *Serialx, char *format, ...);
uint8_t SERIAL_GetRxFlag(SERIAL_InitTypeDef *Serialx);
void SERIAL_IRQHandler(SERIAL_InitTypeDef *Serialx);

void Serial1_Init(SERIAL_InitTypeDef *Serialx);
void Serial2_Init(SERIAL_InitTypeDef *Serialx);

#ifdef __cplusplus
}
#endif

#endif
