#ifndef __STM32F10X_ABL_YMODEM_NRF24L01_H
#define __STM32F10X_ABL_YMODEM_NRF24L01_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include "stm32f10x_abl_nrf24l01.h"

uint8_t NRF24L01_YmodemReceiveData(NRF24L01_InitTypeDef *Nrf24l01, uint8_t *Data);

#ifdef __cplusplus
}
#endif

#endif
