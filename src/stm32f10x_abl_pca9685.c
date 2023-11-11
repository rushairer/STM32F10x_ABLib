#include "stm32f10x_abl_pca9685.h"
#include "stm32f10x_abl_delay.h"

void PCA9685_WriteReg(PCA9685_InitTypeDef *PCA9685x, uint8_t Address, uint8_t Value)
{
    I2C_AcknowledgeConfig(PCA9685x->I2Cx, ENABLE);

    I2C_GenerateSTART(PCA9685x->I2Cx, ENABLE);
    while (!I2C_CheckEvent(PCA9685x->I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress(PCA9685x->I2Cx, 0x80, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(PCA9685x->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;
    I2C_SendData(PCA9685x->I2Cx, Address);
    while (!I2C_CheckEvent(PCA9685x->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C_SendData(PCA9685x->I2Cx, Value);
    while (!I2C_CheckEvent(PCA9685x->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;

    I2C_GenerateSTOP(PCA9685x->I2Cx, ENABLE);
}

void PCA9685_ReadReg(PCA9685_InitTypeDef *PCA9685x, uint8_t Address, uint8_t *Value)
{
    I2C_AcknowledgeConfig(PCA9685x->I2Cx, ENABLE);

    I2C_GenerateSTART(PCA9685x->I2Cx, ENABLE);
    while (!I2C_CheckEvent(PCA9685x->I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress(PCA9685x->I2Cx, 0x80, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(PCA9685x->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;
    I2C_SendData(PCA9685x->I2Cx, Address);
    while (!I2C_CheckEvent(PCA9685x->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C_GenerateSTOP(PCA9685x->I2Cx, ENABLE);

    I2C_GenerateSTART(PCA9685x->I2Cx, ENABLE);
    while (!I2C_CheckEvent(PCA9685x->I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
        ;
    // I2C_Send7bitAddress(PCA9685x->I2Cx, (PCA9685_I2C_ADDR << 1) & 0x01, I2C_Direction_Receiver);

    I2C_Send7bitAddress(PCA9685x->I2Cx, 0x81, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(PCA9685x->I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        ;
    I2C_AcknowledgeConfig(PCA9685x->I2Cx, DISABLE);

    while (!I2C_CheckEvent(PCA9685x->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))
        ;

    *Value = I2C_ReceiveData(PCA9685x->I2Cx);

    I2C_GenerateSTOP(PCA9685x->I2Cx, ENABLE);
}

void PCA9685_SetChannel(PCA9685_InitTypeDef *PCA9685x, uint8_t Channel, uint32_t On, uint32_t Off)
{
    I2C_AcknowledgeConfig(PCA9685x->I2Cx, ENABLE);

    I2C_GenerateSTART(PCA9685x->I2Cx, ENABLE);
    while (!I2C_CheckEvent(PCA9685x->I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress(PCA9685x->I2Cx, (PCA9685_I2C_ADDR << 1), I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(PCA9685x->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;
    I2C_SendData(PCA9685x->I2Cx, PCA9685_LED0_ON_L + 4 * Channel);
    while (!I2C_CheckEvent(PCA9685x->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C_SendData(PCA9685x->I2Cx, On & 0xff);
    while (!I2C_CheckEvent(PCA9685x->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C_SendData(PCA9685x->I2Cx, (On >> 8) & 0xff);
    while (!I2C_CheckEvent(PCA9685x->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C_SendData(PCA9685x->I2Cx, Off & 0xff);
    while (!I2C_CheckEvent(PCA9685x->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C_SendData(PCA9685x->I2Cx, (Off >> 8) & 0xff);
    while (!I2C_CheckEvent(PCA9685x->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C_GenerateSTOP(PCA9685x->I2Cx, ENABLE);
}

void PCA9685_Init(
    PCA9685_InitTypeDef *PCA9685x,
    uint32_t RCC_APB1Periph,
    I2C_TypeDef *I2Cx,
    uint32_t RCC_APB2Periph,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_SCL_Pin,
    uint16_t GPIO_SDA_Pin)
{
    PCA9685x->RCC_APB1Periph = RCC_APB1Periph;
    PCA9685x->I2Cx           = I2Cx;
    PCA9685x->RCC_APB2Periph = RCC_APB2Periph;
    PCA9685x->GPIOx          = GPIOx;
    PCA9685x->GPIO_SCL_Pin   = GPIO_SCL_Pin;
    PCA9685x->GPIO_SDA_Pin   = GPIO_SDA_Pin;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_OD;
    GPIO_InitStruct.GPIO_Pin   = GPIO_SCL_Pin | GPIO_SDA_Pin;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStruct);
    GPIO_SetBits(GPIOx, GPIO_SCL_Pin | GPIO_SDA_Pin);

    I2C_DeInit(I2Cx);
    I2C_InitTypeDef I2C_InitStruct;
    I2C_InitStruct.I2C_Ack                 = I2C_Ack_Enable;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStruct.I2C_ClockSpeed          = 400000;
    I2C_InitStruct.I2C_DutyCycle           = I2C_DutyCycle_2;
    I2C_InitStruct.I2C_Mode                = I2C_Mode_I2C;
    I2C_InitStruct.I2C_OwnAddress1         = 0x77;

    I2C_Init(I2Cx, &I2C_InitStruct);
    I2C_Cmd(I2Cx, ENABLE);
    Delay_ms(800);

    PCA9685_WriteReg(PCA9685x, PCA9685_MODE1, PCA9685_MODE1_SLEEP);
    PCA9685_WriteReg(PCA9685x, PCA9685_PRE_SCALE, 121); // (25000000/4096/50)-1
    PCA9685_WriteReg(PCA9685x, PCA9685_MODE1, 0);
    PCA9685_WriteReg(PCA9685x, PCA9685_MODE1, PCA9685_MODE1_RESTART | PCA9685_MODE1_AI | PCA9685_MODE1_ALLCALL);

    uint8_t value;

    while (1) {
        PCA9685_ReadReg(PCA9685x, PCA9685_MODE1, &value);
        if ((value & PCA9685_MODE1_RESTART) == 0) { // Restart OK.
            break;
        }
        Delay_ms(100);
    }
}

void PCA9685_SetAngle(PCA9685_InitTypeDef *PCA9685x, uint8_t Channel, float Angle)
{
    PCA9685_SetChannel(PCA9685x, Channel, 0, (uint32_t)(4095 * ((Angle / 180.0) * 2.0 + 0.5) / 20.0));
}