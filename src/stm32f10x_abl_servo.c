#include "stm32f10x_abl_servo.h"

void SERVO_Init(
    SERVO_TypeDef *Servox,
    uint32_t RCC_APBxPeriph_TIM,
    TIM_TypeDef *TIMx,
    uint8_t CH,
    uint32_t RCC_APB2Periph_PIN,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_Pin)
{
    Servox->RCC_APBxPeriph_TIM = RCC_APBxPeriph_TIM;
    Servox->TIMx               = TIMx;
    Servox->CH                 = CH;
    Servox->RCC_APB2Periph_PIN = RCC_APB2Periph_PIN;
    Servox->GPIOx              = GPIOx;
    Servox->GPIO_Pin           = GPIO_Pin;

    switch (RCC_APBxPeriph_TIM) {
        case RCC_APB1Periph_TIM12:
        case RCC_APB1Periph_TIM13:
        case RCC_APB1Periph_TIM14:
        case RCC_APB1Periph_TIM2:
        case RCC_APB1Periph_TIM3:
        case RCC_APB1Periph_TIM4:
        case RCC_APB1Periph_TIM5:
        case RCC_APB1Periph_TIM6:
        case RCC_APB1Periph_TIM7:
            RCC_APB1PeriphClockCmd(RCC_APBxPeriph_TIM, ENABLE);
            break;
        default:
            RCC_APB2PeriphClockCmd(RCC_APBxPeriph_TIM, ENABLE);
            break;
    }

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_PIN, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStructure);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period            = 20000 - 1; // ARR
    TIM_TimeBaseInitStructure.TIM_Prescaler         = 72 - 1;    // PSC
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStructure);

    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse       = 0; // CCR

    switch (CH) {
        case 1:
            TIM_OC1Init(TIMx, &TIM_OCInitStructure);
            break;
        case 2:
            TIM_OC2Init(TIMx, &TIM_OCInitStructure);
            break;
        case 3:
            TIM_OC3Init(TIMx, &TIM_OCInitStructure);
            break;
        case 4:
            TIM_OC4Init(TIMx, &TIM_OCInitStructure);
            break;
    }
    TIM_Cmd(TIMx, ENABLE);
}

void SERVO_SetAngle(SERVO_TypeDef *Servox, float Angle)
{
    float newAngle = Angle / 180 * 2000 + 500;
    switch (Servox->CH) {
        case 1:
            TIM_SetCompare1(Servox->TIMx, newAngle);
            break;
        case 2:
            TIM_SetCompare2(Servox->TIMx, newAngle);
            break;
        case 3:
            TIM_SetCompare3(Servox->TIMx, newAngle);
            break;
        case 4:
            TIM_SetCompare4(Servox->TIMx, newAngle);
            break;
    }
}