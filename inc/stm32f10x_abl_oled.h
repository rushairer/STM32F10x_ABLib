#ifndef __STM32F10x_ABL_OLED_H
#define __STM32F10x_ABL_OLED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include "stm32f10x_abl_i2c_soft.h"

typedef enum {
    OLED_COLOR_NORMAL   = 0,
    OlED_COLOR_REVERSED = 1,
} OLED_Color;

typedef struct
{
    I2C_Soft_InitTypeDef *I2C_Soft;

    uint32_t RCC_APB1Periph;
    I2C_TypeDef *I2Cx;
    uint32_t RCC_APB2Periph;
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_SCL_Pin;
    uint16_t GPIO_SDA_Pin;

    uint8_t Buffer[8][128];
    uint8_t IsSoft;

} OLED_InitTypeDef;

void OLED_Soft_Init(OLED_InitTypeDef *Oledx, I2C_Soft_InitTypeDef *I2C_Soft);
void OLED_Device_Init(
    OLED_InitTypeDef *Oledx,
    uint32_t RCC_APB1Periph,
    I2C_TypeDef *I2Cx,
    uint32_t RCC_APB2Periph,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_SCL_Pin,
    uint16_t GPIO_SDA_Pin);

void OLED_Display_On(OLED_InitTypeDef *Oledx);
void OLED_Display_Off(OLED_InitTypeDef *Oledx);
void OLED_RefreshScreen(OLED_InitTypeDef *Oledx);
void OLED_CleanBuffer(OLED_InitTypeDef *Oledx);

void OLED_SetPixel(OLED_InitTypeDef *Oledx, uint8_t X, uint8_t Y, uint8_t Color);
void OLED_SetByte(OLED_InitTypeDef *Oledx, uint8_t Line, uint8_t Column, uint8_t Data, uint8_t Color);
void OLED_DrawLine(OLED_InitTypeDef *Oledx, uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, uint8_t Color);

void OLED_ShowChar(OLED_InitTypeDef *Oledx, uint8_t X, uint8_t Y, char Char, uint8_t Size, uint8_t Color);
void OLED_ShowString(OLED_InitTypeDef *Oledx, uint8_t X, uint8_t Y, char *String, uint8_t Size, uint8_t Color);
void OLED_ShowNumber(OLED_InitTypeDef *Oledx, uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, uint8_t Size, uint8_t Color);
void OLED_DrawImage(OLED_InitTypeDef *Oledx, uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, const uint8_t BMP[]);

void Oled1_Init(OLED_InitTypeDef *Oledx);
void Oled2_Init(OLED_InitTypeDef *Oledx);

#ifdef __cplusplus
}
#endif

#endif
