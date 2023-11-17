#include "stm32f10x_abl_ymodem_nrf24l01.h"
#include "stm32f10x_abl_delay.h"
#include "stm32f10x_abl_ymodem.h"
#include <string.h>
#include <stdio.h>

void NRF24L01_SendByte(NRF24L01_InitTypeDef *Nrf24l01, uint8_t Byte)
{
    NRF24L01_TxMode(Nrf24l01);
    Delay_us(50);

    uint8_t Buf[2] = {0};
    Buf[0]         = 1;
    Buf[1]         = Byte;

    NRF24L01_SendTxBuf(Nrf24l01, Buf);
}

uint8_t NRF24L01_YmodemReceiveDataPackage(NRF24L01_InitTypeDef *Nrf24l01, uint8_t *Data, int32_t *Length)
{
    NRF24L01_RxMode(Nrf24l01);
    Delay_ms(100);
    uint8_t Buf[2048]   = {0};
    uint8_t TempBuf[32] = {0};

    uint32_t theIndex = 0;
    if (!NRF24L01_Get_Value_Flag(Nrf24l01)) {
        while (theIndex < 2048) {
            memset(TempBuf, 1, 32);
            NRF24L01_GetRxBuf(Nrf24l01, TempBuf);
            // Delay_ms(10);
            // NRF24L01_SendByte(Nrf24l01, CRC16);
            // NRF24L01_SendTxBuf(Nrf24l01, Data);

            for (uint8_t i = 0; i < TempBuf[0]; i++) {
                Buf[theIndex] = TempBuf[i + 1];
                theIndex++;
            }

            Data = Buf;
            // if (strlen((char *)Data) > 500) {
            //     printf((char *)Data);
            //     return 1;
            // } else {
            //     // NRF24L01_SendByte(Nrf24l01, CRC16);

            //     // return 0;
            // }
        }

        return 1;
    } else {
        NRF24L01_SendByte(Nrf24l01, CRC16);
        return 0;
    }
}

uint8_t NRF24L01_YmodemReceiveData(NRF24L01_InitTypeDef *Nrf24l01, uint8_t *Data)
{
    NRF24L01_RxMode(Nrf24l01);
    Delay_ms(10);
    // TODO:

    NRF24L01_SendByte(Nrf24l01, CRC16);

    uint8_t Buf[32]      = {0};
    int32_t PacketLength = 0;

    while (NRF24L01_YmodemReceiveDataPackage(Nrf24l01, Buf, &PacketLength) != 1) {
    }

    return 1;
}
