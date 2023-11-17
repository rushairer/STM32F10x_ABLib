#include "stm32f10x_abl_joystick.h"
#include "stm32f10x_abl_delay.h"

void JOYSTICK_Init(
    JOYSTICK_InitTypeDef *Joystickx,
    uint32_t RCC_APB2Periph_ADC,
    ADC_TypeDef *ADCx,
    uint32_t RCC_APB2Periph_PIN,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_X_Pin,
    uint16_t GPIO_Y_Pin,
    uint8_t ADC_Channel_X,
    uint8_t ADC_Channel_Y,
    uint16_t GPIO_Key_Pin)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_PIN, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    Joystickx->RCC_APB2Periph_ADC = RCC_APB2Periph_ADC;
    Joystickx->ADCx               = ADCx;
    Joystickx->RCC_APB2Periph_PIN = RCC_APB2Periph_PIN;
    Joystickx->GPIOx              = GPIOx;
    Joystickx->GPIO_X_Pin         = GPIO_X_Pin;
    Joystickx->GPIO_Y_Pin         = GPIO_Y_Pin;
    Joystickx->ADC_Channel_X      = ADC_Channel_X;
    Joystickx->ADC_Channel_Y      = ADC_Channel_Y;
    Joystickx->GPIO_Key_Pin       = GPIO_Key_Pin;

    GPIO_InitTypeDef GPIO_InitStruct_XY;
    GPIO_InitStruct_XY.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_InitStruct_XY.GPIO_Pin   = GPIO_X_Pin | GPIO_Y_Pin;
    GPIO_InitStruct_XY.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStruct_XY);

    GPIO_InitTypeDef GPIO_InitStruct_Key;
    GPIO_InitStruct_Key.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStruct_Key.GPIO_Pin   = GPIO_Key_Pin;
    GPIO_InitStruct_Key.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStruct_Key);

    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;
    ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ScanConvMode       = DISABLE;
    ADC_InitStructure.ADC_NbrOfChannel       = 2;
    ADC_Init(ADCx, &ADC_InitStructure);

    ADC_Cmd(ADCx, ENABLE);

    ADC_ResetCalibration(ADCx);
    while (ADC_GetResetCalibrationStatus(ADCx) == SET)
        ;
    ADC_StartCalibration(ADCx);
    while (ADC_GetCalibrationStatus(ADCx) == SET)
        ;
}

uint16_t JOYSTICK_GetXValue(JOYSTICK_InitTypeDef *Joystickx)
{
    ADC_RegularChannelConfig(Joystickx->ADCx, Joystickx->ADC_Channel_X, 1, ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(Joystickx->ADCx, ENABLE);
    while (ADC_GetFlagStatus(Joystickx->ADCx, ADC_FLAG_EOC) == RESET)
        ;
    return ADC_GetConversionValue(Joystickx->ADCx);
}

uint16_t JOYSTICK_GetYValue(JOYSTICK_InitTypeDef *Joystickx)
{
    ADC_RegularChannelConfig(Joystickx->ADCx, Joystickx->ADC_Channel_Y, 1, ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(Joystickx->ADCx, ENABLE);
    while (ADC_GetFlagStatus(Joystickx->ADCx, ADC_FLAG_EOC) == RESET)
        ;
    return ADC_GetConversionValue(Joystickx->ADCx);
}

uint8_t JOYSTICK_IsPressed(JOYSTICK_InitTypeDef *Joystickx)
{
    uint8_t isPressed = 0;

    if (GPIO_ReadInputDataBit(Joystickx->GPIOx, Joystickx->GPIO_Key_Pin) == RESET) {
        Delay_ms(20);
        while (GPIO_ReadInputDataBit(Joystickx->GPIOx, Joystickx->GPIO_Key_Pin) == RESET)
            ;
        Delay_ms(20);
        isPressed = 1;
    }
    return isPressed;
}