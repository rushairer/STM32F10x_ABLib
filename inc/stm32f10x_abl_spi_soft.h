#ifndef __STM32F10X_ABL_SPI_H
#define __STM32F10X_ABL_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

typedef struct {
    uint32_t RCC_APB2Periph;
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_CSN_Pin;
    uint16_t GPIO_SCK_Pin;
    uint16_t GPIO_MISO_Pin;
    uint16_t GPIO_MOSI_Pin;
} SPI_Soft_InitTypeDef;

void SPI_Soft_Init(
    SPI_Soft_InitTypeDef *SPIx,
    uint32_t RCC_APB2Periph,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_CSN_Pin,
    uint16_t GPIO_SCK_Pin,
    uint16_t GPIO_MISO_Pin,
    uint16_t GPIO_MOSI_Pin);

void SPI_Soft_StartWrite(SPI_Soft_InitTypeDef *SPIx);
void SPI_Soft_StopWrite(SPI_Soft_InitTypeDef *SPIx);
uint8_t SPI_Soft_SwapByte(SPI_Soft_InitTypeDef *SPIx, uint8_t Byte);

void SPI_Soft_WriteCSN(SPI_Soft_InitTypeDef *SPIx, BitAction BitVal);
void SPI_Soft_WriteSCK(SPI_Soft_InitTypeDef *SPIx, BitAction BitVal);
void SPI_Soft_WriteMOSI(SPI_Soft_InitTypeDef *SPIx, BitAction BitVal);
uint8_t SPI_ReadMISO(SPI_Soft_InitTypeDef *SPIx);

#ifdef __cplusplus
}
#endif

#endif
