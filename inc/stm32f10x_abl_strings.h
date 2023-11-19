#ifndef __STM32F10X_ABL_STRINGS_H
#define __STM32F10X_ABL_STRINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

#define IS_AF(c)            ((c >= 'A') && (c <= 'F'))
#define IS_af(c)            ((c >= 'a') && (c <= 'f'))
#define IS_09(c)            ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)       IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)       IS_09(c)
#define CONVERTDEC(c)       (c - '0')
#define CONVERTHEX_alpha(c) (IS_AF(c) ? (c - 'A' + 10) : (c - 'a' + 10))
#define CONVERTHEX(c)       (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

uint32_t STRINGS_Str2Int(uint8_t *inputstr, int32_t *intnum);
void STRINGS_Int2Str(int32_t intnum, uint8_t *str);

#ifdef __cplusplus
}
#endif

#endif
