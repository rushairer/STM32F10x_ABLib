#include "stm32f10x_abl_key.h"
#include "stm32f10x_abl_delay.h"

void KEY_Init(
    KEY_InitTypeDef *Keyx,
    uint32_t RCC_APB2Periph,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_Pin)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph, ENABLE);

    Keyx->RCC_APB2Periph = RCC_APB2Periph;
    Keyx->GPIOx          = GPIOx;
    Keyx->GPIO_Pin       = GPIO_Pin;

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
}

uint8_t KEY_IsPressed(KEY_InitTypeDef *Keyx)
{
    uint8_t isPressed = 0;

    if (GPIO_ReadInputDataBit(Keyx->GPIOx, Keyx->GPIO_Pin) == 0) {
        Delay_ms(20);
        while (GPIO_ReadInputDataBit(Keyx->GPIOx, Keyx->GPIO_Pin) == 0)
            ;
        Delay_ms(20);
        isPressed = 1;
    }
    return isPressed;
}