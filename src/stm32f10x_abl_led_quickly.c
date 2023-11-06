#include "stm32f10x_abl_led.h"

void Led1_Init(LED_InitTypeDef *Ledx)
{
    LED_Init(Ledx, RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_1);
}

void Led2_Init(LED_InitTypeDef *Ledx)
{
    LED_Init(Ledx, RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_2);
}

void Led3_Init(LED_InitTypeDef *Ledx)
{
    LED_Init(Ledx, RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_3);
}