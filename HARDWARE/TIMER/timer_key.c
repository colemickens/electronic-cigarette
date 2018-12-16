#include "timer_key.h"
#include "UI_manager.h"
#include "oled_content_muxlock.h"

void timer_key_init()
{
    TIMER_BaseInitPara  TIM_TimeBaseStructure;
    NVIC_InitPara NVIC_InitStructure;
    RCC_APB1PeriphClock_Enable(KEY_UPDATE_TIMER_RCC, ENABLE);

    /* TIMER14 configuration */
    //TIMER_DeInit(TIMER15);
    TIM_TimeBaseStructure.TIMER_Prescaler = KEY_PSC;
    TIM_TimeBaseStructure.TIMER_CounterMode = TIMER_COUNTER_UP;
    TIM_TimeBaseStructure.TIMER_Period = KEY_ARR;
    TIM_TimeBaseStructure.TIMER_ClockDivision = TIMER_CDIV_DIV1;
    TIMER_BaseInit(KEY_UPDATE_TIMER,&TIM_TimeBaseStructure);

    TIMER_INTConfig(KEY_UPDATE_TIMER, TIMER_INT_UPDATE, ENABLE);
    TIMER_Enable(KEY_UPDATE_TIMER, ENABLE);  //ʹ��TIMx����

    NVIC_InitStructure.NVIC_IRQ = KEY_UPDATE_INT;  //TIM5�ж�
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;  //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQSubPriority = 3;  //�����ȼ�3��
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

    RCC_APB2PeriphClock_Enable(KEY_FRESH_TIMER_RCC, ENABLE);

    /* TIMER16 configuration */
    //TIMER_DeInit(TIMER16);
    TIM_TimeBaseStructure.TIMER_Prescaler = KEY_FRESH_PSC;
    TIM_TimeBaseStructure.TIMER_CounterMode = TIMER_COUNTER_UP;
    TIM_TimeBaseStructure.TIMER_Period = KEY_FRESH_ARR;
    TIM_TimeBaseStructure.TIMER_ClockDivision = TIMER_CDIV_DIV1;
    TIMER_BaseInit(KEY_FRESH_TIMER,&TIM_TimeBaseStructure);

    TIMER_Enable(KEY_FRESH_TIMER, DISABLE);  //ʹ��TIMx����
    TIMER_INTConfig(KEY_FRESH_TIMER, TIMER_INT_UPDATE, DISABLE);
    TIMER_ClearIntBitState(KEY_FRESH_TIMER, TIMER_INT_UPDATE);  //���TIMx���жϴ�����λ:TIM �ж�Դ

    NVIC_InitStructure.NVIC_IRQ = KEY_FRESH_INT;  //TIM5�ж�
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;  //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQSubPriority = 3;  //�����ȼ�3��
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

void timer_key_deinit()
{
    RCC_APB1PeriphClock_Enable(KEY_UPDATE_TIMER_RCC, DISABLE);
    RCC_APB2PeriphClock_Enable(KEY_FRESH_TIMER_RCC, DISABLE);
    TIMER_Enable(KEY_UPDATE_TIMER, DISABLE);
    TIMER_Enable(TIMER16, DISABLE);
}

void TIMER14_IRQHandler(void)   //TIM15�ж�
{

    if (TIMER_GetIntBitState(KEY_UPDATE_TIMER, TIMER_INT_UPDATE) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ
    {
        TIMER_ClearIntBitState(KEY_UPDATE_TIMER, TIMER_INT_UPDATE);  //���TIMx���жϴ�����λ:TIM �ж�Դ
        keys_state_update();
    }
}

void TIMER16_IRQHandler(void)   //TIM16�ж�
{
    u8 smoke_state;
    if (TIMER_GetIntBitState(TIMER16, TIMER_INT_UPDATE) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ
    {
        TIMER_ClearIntBitState(TIMER16, TIMER_INT_UPDATE);  //���TIMx���жϴ�����λ:TIM �ж�Դ
        TIMER_Enable(TIMER16, DISABLE);

        set_system_lock(SYS_UNLOCK);
        smoke_state = get_system_smoke();
        if ((smoke_state != SYS_SMOKE) && (get_oled_lock() != OLED_LOCKED))
            draw_current_screen();
    }
}

void ENABLE_FRESH_SCREEN_TIMER(u8 lock)
{
    if (lock == SYS_LOCK) {
        set_system_lock(lock);
    }
    TIMER_INTConfig(KEY_FRESH_TIMER, TIMER_INT_UPDATE, ENABLE);
    TIMER_SetCounter(KEY_FRESH_TIMER, 0);
    TIMER_Enable(KEY_FRESH_TIMER, ENABLE);
}
