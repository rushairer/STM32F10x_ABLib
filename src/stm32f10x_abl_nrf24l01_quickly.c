#include "stm32f10x_abl_nrf24l01.h"

void Nrf24l011_Init(NRF24L01_InitTypeDef *Nrf24l01, SPI_Soft_InitTypeDef *SPIx)
{
    NRF24L01_Init(
        Nrf24l01,
        SPIx,
        RCC_APB2Periph_GPIOB,
        GPIOB,
        GPIO_Pin_1,
        GPIO_Pin_0);
}