#ifndef __STM32F10x_ABL_DELAY_H
#define __STM32F10x_ABL_DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
void Delay_s(uint32_t s);

#ifdef __cplusplus
}
#endif

#endif
