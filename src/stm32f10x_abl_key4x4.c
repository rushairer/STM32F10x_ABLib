#include "stm32f10x_abl_key4x4.h"
#include "stm32f10x_abl_delay.h"

void KEY4X4_Init(
    KEY4X4_InitTypeDef *Key4x4,
    uint32_t RCC_APB2Periph_R,
    GPIO_TypeDef *GPIOx_R,
    uint16_t GPIO_Pin_R1,
    uint16_t GPIO_Pin_R2,
    uint16_t GPIO_Pin_R3,
    uint16_t GPIO_Pin_R4,
    uint32_t RCC_APB2Periph_C,
    GPIO_TypeDef *GPIOx_C,
    uint16_t GPIO_Pin_C1,
    uint16_t GPIO_Pin_C2,
    uint16_t GPIO_Pin_C3,
    uint16_t GPIO_Pin_C4)
{
    Key4x4->RCC_APB2Periph_R = RCC_APB2Periph_R;
    Key4x4->GPIOx_R          = GPIOx_R;
    Key4x4->GPIO_Pin_R1      = GPIO_Pin_R1;
    Key4x4->GPIO_Pin_R2      = GPIO_Pin_R2;
    Key4x4->GPIO_Pin_R3      = GPIO_Pin_R3;
    Key4x4->GPIO_Pin_R4      = GPIO_Pin_R4;

    Key4x4->RCC_APB2Periph_C = RCC_APB2Periph_C;
    Key4x4->GPIOx_C          = GPIOx_C;
    Key4x4->GPIO_Pin_C1      = GPIO_Pin_C1;
    Key4x4->GPIO_Pin_C2      = GPIO_Pin_C2;
    Key4x4->GPIO_Pin_C3      = GPIO_Pin_C3;
    Key4x4->GPIO_Pin_C4      = GPIO_Pin_C4;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_R, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_C, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure_R;
    GPIO_InitStructure_R.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure_R.GPIO_Pin   = GPIO_Pin_R1 | GPIO_Pin_R2 | GPIO_Pin_R3 | GPIO_Pin_R4;
    GPIO_InitStructure_R.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx_R, &GPIO_InitStructure_R);

    GPIO_InitTypeDef GPIO_InitStructure_C;
    GPIO_InitStructure_C.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure_C.GPIO_Pin   = GPIO_Pin_C1 | GPIO_Pin_C2 | GPIO_Pin_C3 | GPIO_Pin_C4;
    GPIO_InitStructure_C.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx_C, &GPIO_InitStructure_C);

    GPIO_ResetBits(GPIOx_R, GPIO_Pin_R1 | GPIO_Pin_R2 | GPIO_Pin_R3 | GPIO_Pin_R4);
}

uint16_t KEY4X4_GetKey(KEY4X4_InitTypeDef *Key4x4)
{
    uint16_t KeyValue = 0;

    GPIO_SetBits(Key4x4->GPIOx_R, Key4x4->GPIO_Pin_R1 | Key4x4->GPIO_Pin_R2 | Key4x4->GPIO_Pin_R3 | Key4x4->GPIO_Pin_R4);

    if ((GPIO_ReadInputData(Key4x4->GPIOx_R) & 0xff) != 0xff) {
        Delay_ms(10);
        if ((GPIO_ReadInputData(Key4x4->GPIOx_R) & 0xff) != 0xff) {
            uint16_t allData_R = GPIO_ReadInputData(Key4x4->GPIOx_R);
            uint16_t allData_C = GPIO_ReadInputData(Key4x4->GPIOx_C);

            GPIO_ResetBits(Key4x4->GPIOx_R, Key4x4->GPIO_Pin_R1);
            GPIO_SetBits(Key4x4->GPIOx_R, Key4x4->GPIO_Pin_R2 | Key4x4->GPIO_Pin_R3 | Key4x4->GPIO_Pin_R4);

            if (((allData_R ^ GPIO_ReadInputData(Key4x4->GPIOx_R)) & Key4x4->GPIO_Pin_R1) == Key4x4->GPIO_Pin_R1) {
                if (((allData_C ^ GPIO_ReadInputData(Key4x4->GPIOx_C)) & Key4x4->GPIO_Pin_C1) == Key4x4->GPIO_Pin_C1) {
                    KeyValue = KeyValue | 1;
                }
                if (((allData_C ^ GPIO_ReadInputData(Key4x4->GPIOx_C)) & Key4x4->GPIO_Pin_C2) == Key4x4->GPIO_Pin_C2) {
                    KeyValue = KeyValue | 1 << 1;
                }
                if (((allData_C ^ GPIO_ReadInputData(Key4x4->GPIOx_C)) & Key4x4->GPIO_Pin_C3) == Key4x4->GPIO_Pin_C3) {
                    KeyValue = KeyValue | 1 << 2;
                }
                if (((allData_C ^ GPIO_ReadInputData(Key4x4->GPIOx_C)) & Key4x4->GPIO_Pin_C4) == Key4x4->GPIO_Pin_C4) {
                    KeyValue = KeyValue | 1 << 3;
                }
            }

            GPIO_ResetBits(Key4x4->GPIOx_R, Key4x4->GPIO_Pin_R2);
            GPIO_SetBits(Key4x4->GPIOx_R, Key4x4->GPIO_Pin_R1 | Key4x4->GPIO_Pin_R3 | Key4x4->GPIO_Pin_R4);

            if (((allData_R ^ GPIO_ReadInputData(Key4x4->GPIOx_R)) & Key4x4->GPIO_Pin_R2) == Key4x4->GPIO_Pin_R2) {
                if (((allData_C ^ GPIO_ReadInputData(Key4x4->GPIOx_C)) & Key4x4->GPIO_Pin_C1) == Key4x4->GPIO_Pin_C1) {
                    KeyValue = KeyValue | 1 << 4;
                }
                if (((allData_C ^ GPIO_ReadInputData(Key4x4->GPIOx_C)) & Key4x4->GPIO_Pin_C2) == Key4x4->GPIO_Pin_C2) {
                    KeyValue = KeyValue | 1 << 5;
                }
                if (((allData_C ^ GPIO_ReadInputData(Key4x4->GPIOx_C)) & Key4x4->GPIO_Pin_C3) == Key4x4->GPIO_Pin_C3) {
                    KeyValue = KeyValue | 1 << 6;
                }
                if (((allData_C ^ GPIO_ReadInputData(Key4x4->GPIOx_C)) & Key4x4->GPIO_Pin_C4) == Key4x4->GPIO_Pin_C4) {
                    KeyValue = KeyValue | 1 << 7;
                }
            }

            GPIO_ResetBits(Key4x4->GPIOx_R, Key4x4->GPIO_Pin_R3);
            GPIO_SetBits(Key4x4->GPIOx_R, Key4x4->GPIO_Pin_R2 | Key4x4->GPIO_Pin_R1 | Key4x4->GPIO_Pin_R4);

            if (((allData_R ^ GPIO_ReadInputData(Key4x4->GPIOx_R)) & Key4x4->GPIO_Pin_R3) == Key4x4->GPIO_Pin_R3) {
                if (((allData_C ^ GPIO_ReadInputData(Key4x4->GPIOx_C)) & Key4x4->GPIO_Pin_C1) == Key4x4->GPIO_Pin_C1) {
                    KeyValue = KeyValue | 1 << 8;
                }
                if (((allData_C ^ GPIO_ReadInputData(Key4x4->GPIOx_C)) & Key4x4->GPIO_Pin_C2) == Key4x4->GPIO_Pin_C2) {
                    KeyValue = KeyValue | 1 << 9;
                }
                if (((allData_C ^ GPIO_ReadInputData(Key4x4->GPIOx_C)) & Key4x4->GPIO_Pin_C3) == Key4x4->GPIO_Pin_C3) {
                    KeyValue = KeyValue | 1 << 10;
                }
                if (((allData_C ^ GPIO_ReadInputData(Key4x4->GPIOx_C)) & Key4x4->GPIO_Pin_C4) == Key4x4->GPIO_Pin_C4) {
                    KeyValue = KeyValue | 1 << 11;
                }
            }

            GPIO_ResetBits(Key4x4->GPIOx_R, Key4x4->GPIO_Pin_R4);
            GPIO_SetBits(Key4x4->GPIOx_R, Key4x4->GPIO_Pin_R2 | Key4x4->GPIO_Pin_R3 | Key4x4->GPIO_Pin_R1);

            if (((allData_R ^ GPIO_ReadInputData(Key4x4->GPIOx_R)) & Key4x4->GPIO_Pin_R4) == Key4x4->GPIO_Pin_R4) {
                if (((allData_C ^ GPIO_ReadInputData(Key4x4->GPIOx_C)) & Key4x4->GPIO_Pin_C1) == Key4x4->GPIO_Pin_C1) {
                    KeyValue = KeyValue | 1 << 12;
                }
                if (((allData_C ^ GPIO_ReadInputData(Key4x4->GPIOx_C)) & Key4x4->GPIO_Pin_C2) == Key4x4->GPIO_Pin_C2) {
                    KeyValue = KeyValue | 1 << 13;
                }
                if (((allData_C ^ GPIO_ReadInputData(Key4x4->GPIOx_C)) & Key4x4->GPIO_Pin_C3) == Key4x4->GPIO_Pin_C3) {
                    KeyValue = KeyValue | 1 << 14;
                }
                if (((allData_C ^ GPIO_ReadInputData(Key4x4->GPIOx_C)) & Key4x4->GPIO_Pin_C4) == Key4x4->GPIO_Pin_C4) {
                    KeyValue = KeyValue | 1 << 15;
                }
            }
        }
    }

    return KeyValue;
}