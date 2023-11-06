#include "stm32f10x_abl_oled.h"
#include "stm32f10x_abl_oled_font.h"
#include "stm32f10x_abl_delay.h"
#include "stm32f10x_i2c.h"
#include <stdlib.h>
#include <string.h>

void OLED_WriteCommand(OLED_InitTypeDef *Oledx, uint8_t Command)
{
    if (Oledx->IsSoft == 1) {
        I2C_Soft_Start(Oledx->I2C_Soft);
        I2C_Soft_SendByte(Oledx->I2C_Soft, 0x78);
        I2C_Soft_Wait(Oledx->I2C_Soft);
        I2C_Soft_SendByte(Oledx->I2C_Soft, 0x00);
        I2C_Soft_Wait(Oledx->I2C_Soft);
        I2C_Soft_SendByte(Oledx->I2C_Soft, Command);
        I2C_Soft_Wait(Oledx->I2C_Soft);
        I2C_Soft_Stop(Oledx->I2C_Soft);
    } else {
        while (I2C_GetFlagStatus(Oledx->I2Cx, I2C_FLAG_BUSY))
            ;
        I2C_GenerateSTART(Oledx->I2Cx, ENABLE);
        while (!I2C_CheckEvent(Oledx->I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
            ;
        I2C_Send7bitAddress(Oledx->I2Cx, 0x78, I2C_Direction_Transmitter);
        while (!I2C_CheckEvent(Oledx->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
            ;
        I2C_SendData(Oledx->I2Cx, 0x00);
        while (!I2C_CheckEvent(Oledx->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
            ;
        I2C_SendData(Oledx->I2Cx, Command);
        while (!I2C_CheckEvent(Oledx->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
            ;
        I2C_GenerateSTOP(Oledx->I2Cx, ENABLE);
    }
}

void OLED_WriteData(OLED_InitTypeDef *Oledx, uint8_t Data)
{
    if (Oledx->IsSoft == 1) {
        I2C_Soft_Start(Oledx->I2C_Soft);
        I2C_Soft_SendByte(Oledx->I2C_Soft, 0x78);
        I2C_Soft_Wait(Oledx->I2C_Soft);
        I2C_Soft_SendByte(Oledx->I2C_Soft, 0x40);
        I2C_Soft_Wait(Oledx->I2C_Soft);
        I2C_Soft_SendByte(Oledx->I2C_Soft, Data);
        I2C_Soft_Wait(Oledx->I2C_Soft);
        I2C_Soft_Stop(Oledx->I2C_Soft);
    } else {
        while (I2C_GetFlagStatus(Oledx->I2Cx, I2C_FLAG_BUSY))
            ;
        I2C_GenerateSTART(Oledx->I2Cx, ENABLE);
        while (!I2C_CheckEvent(Oledx->I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
            ;
        I2C_Send7bitAddress(Oledx->I2Cx, 0x78, I2C_Direction_Transmitter);
        while (!I2C_CheckEvent(Oledx->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
            ;
        I2C_SendData(Oledx->I2Cx, 0x40);
        while (!I2C_CheckEvent(Oledx->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
            ;
        I2C_SendData(Oledx->I2Cx, Data);
        while (!I2C_CheckEvent(Oledx->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
            ;
        I2C_GenerateSTOP(Oledx->I2Cx, ENABLE);
    }
}

void OLED_SetCursor(OLED_InitTypeDef *Oledx, uint8_t X, uint8_t Y)
{
    OLED_WriteCommand(Oledx, 0xB0 + Y);                 // 设置Y位置
    OLED_WriteCommand(Oledx, 0X10 | ((X & 0xF0) >> 4)); // 设置X位置高4位
    OLED_WriteCommand(Oledx, 0x00 | (X & 0x0F));        // 设置X位置低4位
}

void OLED_Int(OLED_InitTypeDef *Oledx)
{
    OLED_WriteCommand(Oledx, 0xAE); // 关闭显示
    OLED_WriteCommand(Oledx, 0x00); // set low column address
    OLED_WriteCommand(Oledx, 0X10); // set high column address

    OLED_WriteCommand(Oledx, 0x40); // 设置显示开始行
    OLED_WriteCommand(Oledx, 0xB0); // set page address

    OLED_WriteCommand(Oledx, 0x81); // 设置对比度控制
    OLED_WriteCommand(Oledx, 0x60);

    OLED_WriteCommand(Oledx, 0xA1); // 设置左右方向，0xA1正常 0xA0左右反置
    OLED_WriteCommand(Oledx, 0xA6); // 设置正常/倒转显示
    OLED_WriteCommand(Oledx, 0xA8); // 设置多路复用率
    OLED_WriteCommand(Oledx, 0x3F); // 1/32 duty
    OLED_WriteCommand(Oledx, 0xC8); // 设置上下方向，0xC8正常 0xC0上下反置

    OLED_WriteCommand(Oledx, 0xD3); // 设置显示偏移
    OLED_WriteCommand(Oledx, 0x00);

    OLED_WriteCommand(Oledx, 0xD5); // 设置显示时钟分频比/振荡器频率
    OLED_WriteCommand(Oledx, 0x80);

    OLED_WriteCommand(Oledx, 0xD8); // set area Color mode off
    OLED_WriteCommand(Oledx, 0x05);

    OLED_WriteCommand(Oledx, 0xD9); // 设置预充电周期
    OLED_WriteCommand(Oledx, 0xF1);

    OLED_WriteCommand(Oledx, 0xDA); // 设置COM引脚硬件配置
    OLED_WriteCommand(Oledx, 0X12);

    OLED_WriteCommand(Oledx, 0xDB); // 设置VCOMH取消选择级别
    OLED_WriteCommand(Oledx, 0x30);

    OLED_WriteCommand(Oledx, 0x8D); // 设置充电泵
    OLED_WriteCommand(Oledx, 0X14);

    OLED_WriteCommand(Oledx, 0xAF); // 开启显示

    OLED_CleanBuffer(Oledx);
    OLED_RefreshScreen(Oledx);
}

void OLED_Soft_Init(OLED_InitTypeDef *Oledx, I2C_Soft_InitTypeDef *I2C_Soft)
{
    Oledx->I2C_Soft = I2C_Soft;
    Oledx->IsSoft   = 1;
    Delay_ms(800);
    OLED_Int(Oledx);
}

void OLED_Device_Init(
    OLED_InitTypeDef *Oledx,
    uint32_t RCC_APB1Periph,
    I2C_TypeDef *I2Cx,
    uint32_t RCC_APB2Periph,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_SCL_Pin,
    uint16_t GPIO_SDA_Pin)
{
    Oledx->RCC_APB1Periph = RCC_APB1Periph;
    Oledx->I2Cx           = I2Cx;

    Oledx->RCC_APB2Periph = RCC_APB2Periph;
    Oledx->GPIOx          = GPIOx;
    Oledx->GPIO_SCL_Pin   = GPIO_SCL_Pin;
    Oledx->GPIO_SDA_Pin   = GPIO_SDA_Pin;

    Oledx->IsSoft = 0;

    // Init
    RCC_APB1PeriphClockCmd(RCC_APB1Periph, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Pin   = GPIO_SCL_Pin | GPIO_SDA_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStructure);

    I2C_DeInit(I2Cx);
    I2C_InitTypeDef I2C_InitStructure;
    I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed          = 400000;
    I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
    I2C_InitStructure.I2C_OwnAddress1         = 0x30;

    I2C_Init(I2Cx, &I2C_InitStructure);
    I2C_Cmd(I2Cx, ENABLE);

    Delay_ms(800);
    OLED_Int(Oledx);
}

void OLED_Display_On(OLED_InitTypeDef *Oledx)
{
    OLED_WriteCommand(Oledx, 0x8D);
    OLED_WriteCommand(Oledx, 0X14);
    OLED_WriteCommand(Oledx, 0xAF);
}

void OLED_Display_Off(OLED_InitTypeDef *Oledx)
{
    OLED_WriteCommand(Oledx, 0x8D);
    OLED_WriteCommand(Oledx, 0X10);
    OLED_WriteCommand(Oledx, 0xAE);
}

void OLED_CleanBuffer(OLED_InitTypeDef *Oledx)
{
    memset(Oledx->Buffer, 0, sizeof(Oledx->Buffer));
}

void OLED_RefreshScreen(OLED_InitTypeDef *Oledx)
{
    uint8_t i, j;
    for (i = 0; i < 8; i++) {
        OLED_SetCursor(Oledx, 0, i);
        for (j = 0; j < 128; j++) {
            OLED_WriteData(Oledx, Oledx->Buffer[i][j]);
        }
    }
}

void OLED_SetPixel(OLED_InitTypeDef *Oledx, uint8_t X, uint8_t Y, uint8_t Color)
{
    if (X >= 128 || Y >= 64) {
        return;
    }

    if (Color) {
        Oledx->Buffer[Y / 8][X] &= ~(1 << (Y % 8));
    } else {
        Oledx->Buffer[Y / 8][X] |= 1 << (Y % 8);
    }
}

void OLED_SetByte(OLED_InitTypeDef *Oledx, uint8_t Line, uint8_t Column, uint8_t Data, uint8_t Color)
{
    if (Line >= 8 || Column >= 128) {
        return;
    }

    if (Color) {
        Data = ~Data;
    }

    Oledx->Buffer[Line][Column] = Data;
}

void OLED_DrawLine(OLED_InitTypeDef *Oledx, uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, uint8_t Color)
{
    static uint8_t temp = 0;
    if (X1 == X2) {
        if (Y1 > Y2) {
            temp = Y1;
            Y1   = Y2;
            Y2   = temp;
        }
        for (uint8_t y = Y1; y <= Y2; y++) {
            OLED_SetPixel(Oledx, X1, y, Color);
        }
    } else if (Y1 == Y2) {
        if (X1 > X2) {
            temp = X1;
            X1   = X2;
            X2   = temp;
        }
        for (uint8_t x = X1; x <= X2; x++) {
            OLED_SetPixel(Oledx, x, Y1, Color);
        }
    } else {
        // Bresenham直线算法
        int16_t dx = X2 - X1;
        int16_t dy = Y2 - Y1;
        int16_t ux = ((dx > 0) << 1) - 1;
        int16_t uy = ((dy > 0) << 1) - 1;
        int16_t x = X1, y = Y1, eps = 0;
        dx = abs(dx);
        dy = abs(dy);
        if (dx > dy) {
            for (x = X1; x != X2; x += ux) {
                OLED_SetPixel(Oledx, x, y, Color);
                eps += dy;
                if ((eps << 1) >= dx) {
                    y += uy;
                    eps -= dx;
                }
            }
        } else {
            for (y = Y1; y != Y2; y += uy) {
                OLED_SetPixel(Oledx, x, y, Color);
                eps += dx;
                if ((eps << 1) >= dy) {
                    x += ux;
                    eps -= dy;
                }
            }
        }
    }
}

void OLED_ShowChar(OLED_InitTypeDef *Oledx, uint8_t X, uint8_t Y, char Char, uint8_t Size, uint8_t Color)
{
    uint8_t temp, i, j;
    uint8_t y0 = Y;
    Char       = Char - ' ';
    for (i = 0; i < Size; i++) {
        switch (Size) {
            case 12:
                temp = OLED_F6x12[(uint8_t)Char][i];
                break;
            default:
                temp = OLED_F8x16[(uint8_t)Char][i];
                break;
        }

        for (j = 0; j < 8; j++) {
            if (temp & 0x80) {
                OLED_SetPixel(Oledx, X, Y, Color);
            } else {
                OLED_SetPixel(Oledx, X, Y, !Color);
            }
            temp <<= 1;
            Y++;
            if ((Y - y0) == Size) {
                Y = y0;
                X++;
                break;
            }
        }
    }
}

uint8_t OLED_FontWidth(OLED_InitTypeDef *Oledx, uint8_t Size)
{
    uint8_t width;
    switch (Size) {
        case 12:
            width = 6;
            break;

        default:
            width = 8;
            break;
    }

    return width;
}

uint32_t OLED_Pow(OLED_InitTypeDef *Oledx, uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--) {
        Result *= X;
    }
    return Result;
}

void OLED_ShowString(OLED_InitTypeDef *Oledx, uint8_t X, uint8_t Y, char *String, uint8_t Size, uint8_t Color)
{
    uint8_t i, width;

    width = OLED_FontWidth(Oledx, Size);

    for (i = 0; String[i] != '\0'; i++) {
        OLED_ShowChar(Oledx, X + i * width, Y, String[i], Size, Color);
    }
}

void OLED_ShowNumber(OLED_InitTypeDef *Oledx, uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, uint8_t Size, uint8_t Color)
{
    uint8_t i, width;
    width = OLED_FontWidth(Oledx, Size);
    for (i = 0; i < Length; i++) {
        OLED_ShowChar(Oledx, X + i * width, Y, Number / OLED_Pow(Oledx, 10, Length - i - 1) % 10 + '0', Size, Color);
    }
}

void OLED_DrawImage(OLED_InitTypeDef *Oledx, uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, const uint8_t BMP[])
{
    uint16_t i = 0, X, Y;
    for (Y = Y1; Y < (Y2 + 1) / 8; Y++) {
        for (X = X1; X < X2 + 1; X++) {
            Oledx->Buffer[Y][X] = BMP[i++];
        }
    }
}
