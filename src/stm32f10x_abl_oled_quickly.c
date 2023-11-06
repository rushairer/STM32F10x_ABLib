#include "stm32f10x_abl_oled.h"

void Oled1_Init(OLED_InitTypeDef *Oledx)
{
    OLED_Device_Init(Oledx, RCC_APB1Periph_I2C1, I2C1, RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_6, GPIO_Pin_7);
}

void Oled2_Init(OLED_InitTypeDef *Oledx)
{
    OLED_Device_Init(Oledx, RCC_APB1Periph_I2C2, I2C2, RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_10, GPIO_Pin_11);
}
