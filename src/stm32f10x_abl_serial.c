#include "stm32f10x_abl_serial.h"
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void SERIAL_Init(
    SERIAL_InitTypeDef *Serialx,
    uint32_t RCC_APBxPeriph_USART,
    USART_TypeDef *USARTx,
    uint32_t RCC_APB2Periph_PIN,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_Tx_Pin,
    uint16_t GPIO_Rx_Pin,
    uint32_t BaudRate)
{
    Serialx->RCC_APBxPeriph_USART = RCC_APBxPeriph_USART;
    Serialx->USARTx               = USARTx;
    Serialx->RCC_APB2Periph_PIN   = RCC_APB2Periph_PIN;
    Serialx->GPIOx                = GPIOx;
    Serialx->GPIO_Tx_Pin          = GPIO_Tx_Pin;
    Serialx->GPIO_Rx_Pin          = GPIO_Rx_Pin;
    Serialx->BaudRate             = BaudRate;

    switch (RCC_APBxPeriph_USART) {
        case RCC_APB2Periph_USART1:
            RCC_APB2PeriphClockCmd(RCC_APBxPeriph_USART, ENABLE);
            break;
        case RCC_APB1Periph_USART2:
            RCC_APB1PeriphClockCmd(RCC_APBxPeriph_USART, ENABLE);
            break;
        case RCC_APB1Periph_USART3:
            RCC_APB1PeriphClockCmd(RCC_APBxPeriph_USART, ENABLE);
            break;
        default:
            RCC_APB1PeriphClockCmd(RCC_APBxPeriph_USART, ENABLE);
            break;
    }

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_PIN, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Pin   = GPIO_Tx_Pin;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin   = GPIO_Rx_Pin;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStruct);

    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate            = BaudRate;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_Parity              = USART_Parity_No;
    USART_InitStruct.USART_StopBits            = USART_StopBits_1;
    USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
    USART_Init(USARTx, &USART_InitStruct);

    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
    USART_Cmd(USARTx, ENABLE);
}

void SERIAL_SendByte(SERIAL_InitTypeDef *Serialx, uint8_t Byte)
{
    USART_SendData(Serialx->USARTx, Byte);
    while (USART_GetFlagStatus(Serialx->USARTx, USART_FLAG_TXE) == RESET) {
        ;
    }
}

uint32_t SERIAL_ReceiveByte(SERIAL_InitTypeDef *Serialx, uint8_t *Byte)
{
    if (USART_GetFlagStatus(Serialx->USARTx, USART_FLAG_RXNE) != RESET) {
        *Byte = (uint8_t)Serialx->USARTx->DR;
        return 1;
    } else {
        return 0;
    }
}

void SERIAL_SendArray(SERIAL_InitTypeDef *Serialx, uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i++) {
        SERIAL_SendByte(Serialx, Array[i]);
    }
}

void SERIAL_SendData(SERIAL_InitTypeDef *Serialx, uint8_t *Array, uint16_t Length)
{
    SERIAL_SendByte(Serialx, 0xFF);
    SERIAL_SendArray(Serialx, Array, Length);
    SERIAL_SendByte(Serialx, 0xFE);
}

void SERIAL_SendString(SERIAL_InitTypeDef *Serialx, char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++) {
        SERIAL_SendByte(Serialx, String[i]);
    }
}

void SERIAL_SendNumber(SERIAL_InitTypeDef *Serialx, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++) {
        SERIAL_SendByte(Serialx, Number / (uint32_t)pow(10, Length - i - 1) % 10 + '0');
    }
}

void SERIAL_Printf(SERIAL_InitTypeDef *Serialx, char *format, ...)
{
    char String[100];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    SERIAL_SendString(Serialx, String);
}
