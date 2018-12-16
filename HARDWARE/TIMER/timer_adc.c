#include "timer_adc.h"
#include "PID.h"


void timer_adc_init()
{
    TIMER_BaseInitPara  TIM_TimeBaseStructure;
    NVIC_InitPara NVIC_InitStructure;

    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_TIMER17, ENABLE);

    /* TIMER17 configuration */
    // TIMER_DeInit(TIMER17);
    TIM_TimeBaseStructure.TIMER_Prescaler = ADC_PSC;
    TIM_TimeBaseStructure.TIMER_CounterMode = TIMER_COUNTER_UP;
    TIM_TimeBaseStructure.TIMER_Period = ADC_ARR;
    TIM_TimeBaseStructure.TIMER_ClockDivision = TIMER_CDIV_DIV1;
    TIMER_BaseInit(TIMER17,&TIM_TimeBaseStructure);

    TIMER_INTConfig(TIMER17, TIMER_INT_UPDATE, ENABLE);
    TIMER_Enable(TIMER17, ENABLE);  //ʹ��TIMx����

    NVIC_InitStructure.NVIC_IRQ = TIMER17_IRQn;  //TIM5�ж�
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;  //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQSubPriority = 1;  //�����ȼ�3��
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

}

void timer_adc_deinit()
{
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_TIMER17, DISABLE);
    TIMER_Enable(TIMER17, DISABLE);  //ʹ��TIMx����
}

void TIMER17_IRQHandler(void)   //TIM17�ж�
{
    //static char cnt;
    if ( TIMER_GetIntBitState(TIMER17 , TIMER_INT_UPDATE) != RESET )
    {
        // Smoke PID control;
        Smoke_Excecute();
        TIMER_ClearIntBitState(TIMER17,TIMER_INT_UPDATE);
    }
}
