#include "stm32f10x_abl_common.h"

uint32_t STM32F10X_ABL_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--) {
        Result *= X;
    }
    return Result;
}