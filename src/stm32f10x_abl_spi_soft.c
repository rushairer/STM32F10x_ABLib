#include "stm32f10x_abl_spi_soft.h"

void SPI_Soft_WriteCSN(SPI_Soft_InitTypeDef *SPIx, BitAction BitVal)
{
    GPIO_WriteBit(SPIx->GPIOx, SPIx->GPIO_CSN_Pin, BitVal);
}

void SPI_Soft_WriteSCK(SPI_Soft_InitTypeDef *SPIx, BitAction BitVal)
{
    GPIO_WriteBit(SPIx->GPIOx, SPIx->GPIO_SCK_Pin, BitVal);
}

void SPI_Soft_WriteMOSI(SPI_Soft_InitTypeDef *SPIx, BitAction BitVal)
{
    GPIO_WriteBit(SPIx->GPIOx, SPIx->GPIO_MOSI_Pin, BitVal);
}

uint8_t SPI_ReadMISO(SPI_Soft_InitTypeDef *SPIx)
{
    return GPIO_ReadInputDataBit(SPIx->GPIOx, SPIx->GPIO_MISO_Pin);
}

void SPI_Soft_Init(
    SPI_Soft_InitTypeDef *SPIx,
    uint32_t RCC_APB2Periph,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_CSN_Pin,
    uint16_t GPIO_SCK_Pin,
    uint16_t GPIO_MISO_Pin,
    uint16_t GPIO_MOSI_Pin)
{
    SPIx->RCC_APB2Periph = RCC_APB2Periph;
    SPIx->GPIOx          = GPIOx;
    SPIx->GPIO_CSN_Pin   = GPIO_CSN_Pin;
    SPIx->GPIO_SCK_Pin   = GPIO_SCK_Pin;
    SPIx->GPIO_MISO_Pin  = GPIO_MISO_Pin;
    SPIx->GPIO_MOSI_Pin  = GPIO_MOSI_Pin;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin   = GPIO_CSN_Pin | GPIO_SCK_Pin | GPIO_MOSI_Pin;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin   = GPIO_MISO_Pin;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void SPI_Soft_StartWrite(SPI_Soft_InitTypeDef *SPIx)
{
    SPI_Soft_WriteCSN(SPIx, Bit_RESET);
}

void SPI_Soft_StopWrite(SPI_Soft_InitTypeDef *SPIx)
{
    SPI_Soft_WriteCSN(SPIx, Bit_SET);
}

uint8_t SPI_Soft_SwapByte(SPI_Soft_InitTypeDef *SPIx, uint8_t Byte)
{
    uint8_t i, ByteReceive = 0x00;

    for (i = 0; i < 8; i++) {
        SPI_Soft_WriteMOSI(SPIx, Byte & (0x80 >> i));
        SPI_Soft_WriteSCK(SPIx, Bit_SET);
        if (SPI_ReadMISO(SPIx) == 1) {
            ByteReceive |= (0x80 >> i);
        }
        SPI_Soft_WriteSCK(SPIx, Bit_RESET);
    }

    return ByteReceive;
}