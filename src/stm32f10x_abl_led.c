#include "stm32f10x_abl_led.h"

void LED_Init(
    LED_InitTypeDef *Ledx,
    uint32_t RCC_APB2Periph,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_Pin)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph, ENABLE);

    Ledx->RCC_APB2Periph = RCC_APB2Periph;
    Ledx->GPIOx          = GPIOx;
    Ledx->GPIO_Pin       = GPIO_Pin;

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
}

uint8_t LED_IsOn(LED_InitTypeDef *Ledx)
{
    if (GPIO_ReadOutputDataBit(Ledx->GPIOx, Ledx->GPIO_Pin) == 0) {
        return 1;
    } else {
        return 0;
    }
}

void LED_On(LED_InitTypeDef *Ledx)
{
    GPIO_ResetBits(Ledx->GPIOx, Ledx->GPIO_Pin);
}

void LED_Off(LED_InitTypeDef *Ledx)
{
    GPIO_SetBits(Ledx->GPIOx, Ledx->GPIO_Pin);
}

void LED_Toggle(LED_InitTypeDef *Ledx)
{
    if (LED_IsOn(Ledx) == 1) {
        LED_Off(Ledx);
    } else {
        LED_On(Ledx);
    }
}