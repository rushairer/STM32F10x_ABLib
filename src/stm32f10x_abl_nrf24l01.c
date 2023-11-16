#include "stm32f10x_abl_nrf24l01.h"
#include "stm32f10x_abl_delay.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

void NRF24L01_WriteCE(NRF24L01_InitTypeDef *Nrf24l01, BitAction BitVal)
{
    GPIO_WriteBit(Nrf24l01->GPIOx, Nrf24l01->GPIO_CE_Pin, BitVal);
}

uint8_t NRF24L01_ReadIRQ(NRF24L01_InitTypeDef *Nrf24l01)
{
    return GPIO_ReadInputDataBit(Nrf24l01->GPIOx, Nrf24l01->GPIO_IRQ_Pin);
}

const uint8_t TX_ADDRESS[NRF24L01_ADDR_WIDTH] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t RX_ADDRESS[NRF24L01_ADDR_WIDTH] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/*
波特率：115200
目标地址：     0xFF,0xFF,0xFF,0xFF,0xFF
本地接收地址0：0xFF,0xFF,0xFF,0xFF,0xFF
通讯频率：2.456GHz
校验模式：16位CRC校验
发射功率：0dBm
空中传输速率：1Mbps
低噪声放大增益：开启
*/

void NRF24L01_Init(
    NRF24L01_InitTypeDef *Nrf24l01,
    SPI_Soft_InitTypeDef *SPIx,
    uint32_t RCC_APB2Periph,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_IRQ_Pin,
    uint16_t GPIO_CE_Pin)
{
    Nrf24l01->SPIx           = SPIx;
    Nrf24l01->RCC_APB2Periph = RCC_APB2Periph;
    Nrf24l01->GPIOx          = GPIOx;
    Nrf24l01->GPIO_IRQ_Pin   = GPIO_IRQ_Pin;
    Nrf24l01->GPIO_CE_Pin    = GPIO_CE_Pin;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin   = GPIO_CE_Pin;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin  = GPIO_IRQ_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOx, &GPIO_InitStruct);

    while (NRF24L01_Check(Nrf24l01))
        ;
}

uint8_t NRF24L01_Write_Reg(NRF24L01_InitTypeDef *Nrf24l01, uint8_t Address, uint8_t Value)
{
    uint8_t Status;

    SPI_Soft_StartWrite(Nrf24l01->SPIx);
    Status = SPI_Soft_SwapByte(Nrf24l01->SPIx, Address);
    SPI_Soft_SwapByte(Nrf24l01->SPIx, Value);
    SPI_Soft_StopWrite(Nrf24l01->SPIx);

    return Status;
}

uint8_t NRF24L01_Read_Reg(NRF24L01_InitTypeDef *Nrf24l01, uint8_t Address)
{
    uint8_t Value;

    SPI_Soft_StartWrite(Nrf24l01->SPIx);
    SPI_Soft_SwapByte(Nrf24l01->SPIx, Address);

    Value = SPI_Soft_SwapByte(Nrf24l01->SPIx, NRF24L01_CMD_REGISTER_R);
    SPI_Soft_StopWrite(Nrf24l01->SPIx);

    return Value;
}

uint8_t NRF24L01_Read_RegBuf(NRF24L01_InitTypeDef *Nrf24l01, uint8_t Address, uint8_t *Buf, uint8_t Len)
{
    uint8_t Status, i;

    SPI_Soft_StartWrite(Nrf24l01->SPIx);
    Status = SPI_Soft_SwapByte(Nrf24l01->SPIx, Address);
    for (i = 0; i < Len; i++) {
        Buf[i] = SPI_Soft_SwapByte(Nrf24l01->SPIx, NRF24L01_CMD_REGISTER_R);
    }

    SPI_Soft_StopWrite(Nrf24l01->SPIx);

    return Status;
}

uint8_t NRF24L01_Write_RegBuf(NRF24L01_InitTypeDef *Nrf24l01, uint8_t Address, uint8_t *Buf, uint8_t Len)
{
    uint8_t Status, i;

    SPI_Soft_StartWrite(Nrf24l01->SPIx);
    Status = SPI_Soft_SwapByte(Nrf24l01->SPIx, Address);
    Delay_ms(10);
    for (i = 0; i < Len; i++) {
        SPI_Soft_SwapByte(Nrf24l01->SPIx, *Buf++);
    }
    SPI_Soft_StopWrite(Nrf24l01->SPIx);

    return Status;
}

void NRF24L01_RxMode(NRF24L01_InitTypeDef *Nrf24l01)
{
    NRF24L01_WriteCE(Nrf24l01, Bit_RESET);

    NRF24L01_Write_RegBuf(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_RX_ADDR_P0, (uint8_t *)TX_ADDRESS, NRF24L01_ADDR_WIDTH);
    NRF24L01_Write_Reg(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_EN_AA, 0x01);           // Enable Auto.Ack:Pipe0
    NRF24L01_Write_Reg(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_EN_RXADDR, 0x01);       // Enable Pipe0
    NRF24L01_Write_Reg(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_RF_CH, NRF24L01_RF_CH); // Select RF channel
    NRF24L01_Write_Reg(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_RX_PW_P0, NRF24L01_PLOAD_WIDTH);
    NRF24L01_Write_Reg(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_RF_SETUP, 0x07);
    NRF24L01_Write_Reg(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_CONFIG, 0x0F); // Set PWR_UP bit, enable CRC(2 bytes) & Prim:RX. RX_DR enabled..

    NRF24L01_WriteCE(Nrf24l01, Bit_SET);
}

void NRF24L01_TxMode(NRF24L01_InitTypeDef *Nrf24l01)
{
    NRF24L01_WriteCE(Nrf24l01, Bit_RESET);

    NRF24L01_Write_RegBuf(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_TX_ADDR, (uint8_t *)TX_ADDRESS, NRF24L01_ADDR_WIDTH);
    NRF24L01_Write_RegBuf(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_RX_ADDR_P0, (uint8_t *)TX_ADDRESS, NRF24L01_ADDR_WIDTH);

    NRF24L01_Write_Reg(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_EN_AA, 0x01);     // Enable Auto.Ack:Pipe0
    NRF24L01_Write_Reg(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_EN_RXADDR, 0x01); // Enable Pipe0
    NRF24L01_Write_Reg(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_SETUP_RETR, 0x1A);
    NRF24L01_Write_Reg(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_RF_CH, NRF24L01_RF_CH); // Select RF channel
    NRF24L01_Write_Reg(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_RF_SETUP, 0x07);        // AT+RATE=2 传输速率 1
    NRF24L01_Write_Reg(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_CONFIG, 0x0E);          // Set PWR_UP bit, enable CRC(2 bytes) & Prim:RX. RX_DR enabled..

    NRF24L01_WriteCE(Nrf24l01, Bit_SET);
}

uint8_t NRF24L01_Check(NRF24L01_InitTypeDef *Nrf24l01)
{
    uint8_t check_in_buf[5]  = {0x11, 0x22, 0x33, 0x44, 0x55};
    uint8_t check_out_buf[5] = {0x00};

    SPI_Soft_WriteSCK(Nrf24l01->SPIx, Bit_RESET);
    SPI_Soft_WriteCSN(Nrf24l01->SPIx, Bit_SET);
    NRF24L01_WriteCE(Nrf24l01, Bit_RESET);

    NRF24L01_Write_RegBuf(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_TX_ADDR, check_in_buf, NRF24L01_ADDR_WIDTH);
    NRF24L01_Read_RegBuf(Nrf24l01, NRF24L01_CMD_REGISTER_R + NRF24L01_REG_TX_ADDR, check_out_buf, NRF24L01_ADDR_WIDTH);

    if ((check_out_buf[0] == 0x11) && (check_out_buf[1] == 0x22) && (check_out_buf[2] == 0x33) && (check_out_buf[3] == 0x44) && (check_out_buf[4] == 0x55)) {
        return 0;
    } else {
        return 1;
    }
}

uint8_t NRF24L01_Get_Value_Flag(NRF24L01_InitTypeDef *Nrf24l01)
{
    return NRF24L01_ReadIRQ(Nrf24l01);
}

uint8_t NRF24L01_GetRxBuf(NRF24L01_InitTypeDef *Nrf24l01, uint8_t *Buf)
{
    uint8_t State;
    State = NRF24L01_Read_Reg(Nrf24l01, NRF24L01_REG_STATUS);
    NRF24L01_Write_Reg(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_STATUS, State);
    if (State & NRF24L01_FLAG_RX_DREADY) {
        NRF24L01_WriteCE(Nrf24l01, Bit_SET);

        NRF24L01_Read_RegBuf(Nrf24l01, NRF24L01_CMD_RX_PLOAD_R, Buf, NRF24L01_PLOAD_WIDTH);
        NRF24L01_Write_Reg(Nrf24l01, NRF24L01_CMD_FLUSH_RX, NRF24L01_CMD_NOP);

        NRF24L01_WriteCE(Nrf24l01, Bit_SET);
        Delay_us(150);
        return 0;
    }
    return 1;
}

uint8_t NRF24L01_SendTxBuf(NRF24L01_InitTypeDef *Nrf24l01, uint8_t *Buf)
{
    uint8_t State;

    NRF24L01_WriteCE(Nrf24l01, Bit_RESET);
    NRF24L01_Write_RegBuf(Nrf24l01, NRF24L01_CMD_TX_PLOAD_W, Buf, NRF24L01_PLOAD_WIDTH);
    NRF24L01_WriteCE(Nrf24l01, Bit_SET);
    while (NRF24L01_ReadIRQ(Nrf24l01) == 1)
        ;
    State = NRF24L01_Read_Reg(Nrf24l01, NRF24L01_REG_STATUS);
    NRF24L01_Write_Reg(Nrf24l01, NRF24L01_CMD_REGISTER_W + NRF24L01_REG_STATUS, State);
    if (State & NRF24L01_FLAG_MAX_RT) {
        NRF24L01_Write_Reg(Nrf24l01, NRF24L01_CMD_FLUSH_TX, NRF24L01_CMD_NOP);
        return NRF24L01_FLAG_MAX_RT;
    }
    if (State & NRF24L01_FLAG_TX_DSENT) {
        return NRF24L01_FLAG_TX_DSENT;
    }
    return NRF24L01_CMD_NOP;
}

void NRF24L01_SendNumber(NRF24L01_InitTypeDef *Nrf24l01, uint32_t Num)
{
    uint8_t SendBuf[11] = {0};
    SendBuf[0]          = 10;
    for (uint8_t i = 0; i < 10; i++) {
        SendBuf[i + 1] = Num / (uint32_t)pow(10, 9 - i) % 10;
    }

    NRF24L01_SendTxBuf(Nrf24l01, SendBuf);
}

uint32_t NRF24L01_GetNumber(NRF24L01_InitTypeDef *Nrf24l01)
{
    uint32_t Num    = 0;
    uint8_t Buf[32] = {0};
    NRF24L01_GetRxBuf(Nrf24l01, Buf);
    for (uint16_t i = 0; i < 10; i++) {
        Num += Buf[i + 1] * pow(10, 9 - i);
    }

    return Num;
}

void NRF24L01_SendString(NRF24L01_InitTypeDef *Nrf24l01, char *String)
{
    uint8_t SizeOfBuf   = strlen(String);
    uint8_t SendBuf[32] = {0};
    uint8_t SendIndex   = 0;

    for (uint8_t i = 0; i < SizeOfBuf; i++) {
        SendIndex++;
        SendBuf[0]         = SendIndex;
        SendBuf[SendIndex] = String[i];

        if (i == SizeOfBuf - 1 || String[i] == 0 || SendIndex >= 31) {
            NRF24L01_SendTxBuf(Nrf24l01, SendBuf);
            SendIndex = 0;
        }
    }
}

void NRF24L01_GetString(NRF24L01_InitTypeDef *Nrf24l01, char *String)
{
    memset(String, 0, 32);
    uint8_t Buf[32] = {0};
    NRF24L01_GetRxBuf(Nrf24l01, Buf);
    uint8_t Len = Buf[0];

    for (uint8_t i = 0; i < Len; i++) {
        String[i] = Buf[i + 1];
    }
}

void NRF24L01_GetStringWithoutSuffix(NRF24L01_InitTypeDef *Nrf24l01, char *String)
{
    memset(String, 0, 32);
    uint8_t Buf[32] = {0};
    NRF24L01_GetRxBuf(Nrf24l01, Buf);
    uint8_t Len = Buf[0];

    for (uint8_t i = 0; i < Len; i++) {
        String[i] = Buf[i + 1];
        if (i + 3 <= Len && Buf[i + 2] == '\r' && Buf[i + 3] == '\n') {
            break;
        }
    }
}

void NRF24L01_SendData(NRF24L01_InitTypeDef *Nrf24l01, uint8_t *Data)
{
    NRF24L01_TxMode(Nrf24l01);
    Delay_ms(10);

    NRF24L01_SendString(Nrf24l01, (char *)Data);
}