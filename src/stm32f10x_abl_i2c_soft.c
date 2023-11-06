#include "stm32f10x_abl_i2c_soft.h"
#include "stm32f10x_abl_delay.h"

void I2C_Soft_Init(
    I2C_Soft_InitTypeDef *I2CSoftx,
    uint32_t RCC_APB2Periph,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_SCL_Pin,
    uint16_t GPIO_SDA_Pin)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph, ENABLE);

    I2CSoftx->RCC_APB2Periph = RCC_APB2Periph;
    I2CSoftx->GPIOx          = GPIOx;
    I2CSoftx->GPIO_SCL_Pin   = GPIO_SCL_Pin;
    I2CSoftx->GPIO_SDA_Pin   = GPIO_SDA_Pin;

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_SCL_Pin | GPIO_SDA_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
    GPIO_SetBits(GPIOx, GPIO_SCL_Pin | GPIO_SDA_Pin);
}

void I2C_Soft_WriteScl(I2C_Soft_InitTypeDef *I2CSoftx, BitAction BitVal)
{
    GPIO_WriteBit(I2CSoftx->GPIOx, I2CSoftx->GPIO_SCL_Pin, BitVal);
}

void I2C_Soft_WriteSda(I2C_Soft_InitTypeDef *I2CSoftx, BitAction BitVal)
{
    GPIO_WriteBit(I2CSoftx->GPIOx, I2CSoftx->GPIO_SDA_Pin, BitVal);
}

uint8_t I2C_Soft_ReadSda(I2C_Soft_InitTypeDef *I2CSoftx)
{
    uint8_t BitValue;
    BitValue = GPIO_ReadInputDataBit(I2CSoftx->GPIOx, I2CSoftx->GPIO_SDA_Pin);
    Delay_us(10);
    return BitValue;
}

void I2C_Soft_Start(I2C_Soft_InitTypeDef *I2CSoftx)
{
    I2C_Soft_WriteSda(I2CSoftx, Bit_SET);
    I2C_Soft_WriteScl(I2CSoftx, Bit_SET);
    I2C_Soft_WriteSda(I2CSoftx, Bit_RESET);
    I2C_Soft_WriteScl(I2CSoftx, Bit_RESET);
}

void I2C_Soft_Stop(I2C_Soft_InitTypeDef *I2CSoftx)
{
    I2C_Soft_WriteSda(I2CSoftx, Bit_RESET);
    I2C_Soft_WriteScl(I2CSoftx, Bit_SET);
    I2C_Soft_WriteSda(I2CSoftx, Bit_SET);
}

void I2C_Soft_Wait(I2C_Soft_InitTypeDef *I2CSoftx)
{
    I2C_Soft_WriteScl(I2CSoftx, Bit_SET);
    I2C_Soft_WriteScl(I2CSoftx, Bit_RESET);
}

void I2C_Soft_SendByte(I2C_Soft_InitTypeDef *I2CSoftx, uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++) {
        I2C_Soft_WriteSda(I2CSoftx, Byte & (0x80 >> i));
        I2C_Soft_WriteScl(I2CSoftx, Bit_SET);
        I2C_Soft_WriteScl(I2CSoftx, Bit_RESET);
    }
}

uint8_t I2C_Soft_ReceiveByte(I2C_Soft_InitTypeDef *I2CSoftx)
{
    uint8_t i, Byte = 0x00;
    I2C_Soft_WriteSda(I2CSoftx, Bit_SET);
    for (i = 0; i < 8; i++) {
        I2C_Soft_WriteScl(I2CSoftx, Bit_SET);
        if (I2C_Soft_ReadSda(I2CSoftx) == 1) {
            Byte |= (0x80 >> i);
        }
        I2C_Soft_WriteScl(I2CSoftx, Bit_RESET);
    }
    return Byte;
}

void I2C_Soft_SendAck(I2C_Soft_InitTypeDef *I2CSoftx, uint8_t AckBit)
{
    I2C_Soft_WriteSda(I2CSoftx, AckBit);
    I2C_Soft_WriteScl(I2CSoftx, Bit_SET);
    I2C_Soft_WriteScl(I2CSoftx, Bit_RESET);
}

uint8_t I2C_Soft_ReceiveAck(I2C_Soft_InitTypeDef *I2CSoftx)
{
    uint8_t AckBit;
    I2C_Soft_WriteSda(I2CSoftx, Bit_SET);
    I2C_Soft_WriteScl(I2CSoftx, Bit_SET);
    AckBit = I2C_Soft_ReadSda(I2CSoftx);
    I2C_Soft_WriteScl(I2CSoftx, Bit_RESET);

    return AckBit;
}