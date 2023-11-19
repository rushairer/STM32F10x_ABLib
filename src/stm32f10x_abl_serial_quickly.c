#include "stm32f10x_abl_serial.h"

void Serial1_Init(SERIAL_InitTypeDef *Serialx)
{
    SERIAL_Init(
        Serialx,
        RCC_APB2Periph_USART1,
        USART1,
        RCC_APB2Periph_GPIOA,
        GPIOA,
        GPIO_Pin_9,
        GPIO_Pin_10,
        9600);
}

void Serial2_Init(SERIAL_InitTypeDef *Serialx)
{
    SERIAL_Init(
        Serialx,
        RCC_APB2Periph_USART1,
        USART1,
        RCC_APB2Periph_GPIOB,
        GPIOB,
        GPIO_Pin_6,
        GPIO_Pin_7,
        9600);
}