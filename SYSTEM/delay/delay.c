#include "delay.h"
#include "sys.h"

//All rights reserved
//********************************************************************************
//V1.2�޸�˵��
//�������ж��е��ó�����ѭ���Ĵ���
//��ֹ��ʱ��׼ȷ,����do while�ṹ!

//V1.3�޸�˵��
//�����˶�UCOSII��ʱ��֧��.
//���ʹ��ucosII,delay_init���Զ�����SYSTICK��ֵ,ʹ֮��ucos��TICKS_PER_SEC��Ӧ.
//delay_ms��delay_usҲ���������ucos�ĸ���.
//delay_us������ucos��ʹ��,����׼ȷ�Ⱥܸ�,����Ҫ����û��ռ�ö���Ķ�ʱ��.
//delay_ms��ucos��,���Ե���OSTimeDly����,��δ����ucosʱ,������delay_usʵ��,�Ӷ�׼ȷ��ʱ
//����������ʼ������,��������ucos֮��delay_ms������ʱ�ĳ���,ѡ��OSTimeDlyʵ�ֻ���delay_usʵ��.

//V1.4�޸�˵�� 20110929
//�޸���ʹ��ucos,����ucosδ������ʱ��,delay_ms���ж��޷���Ӧ��bug.
//V1.5�޸�˵�� 20120902
//��delay_us����ucos��������ֹ����ucos���delay_us��ִ�У����ܵ��µ���ʱ��׼��
////////////////////////////////////////////////////////////////////////////////// 	 

static u8  fac_us=0;//us��ʱ������
//static u16 fac_ms=0;//ms��ʱ������

//��ʼ���ӳٺ���
//��ʹ��ucos��ʱ��,�˺������ʼ��ucos��ʱ�ӽ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��
void delay_init()	 
{
	SysTick_CKSource_Enable(SYSTICK_CKSOURCE_HCLK_DIV8);	//ѡ���ⲿʱ��  HCLK/8
	fac_us=SystemCoreClock/8000000;	//Ϊϵͳʱ�ӵ�1/8  
	//fac_ms=(u16)fac_us*1000;//��ucos��,����ÿ��ms��Ҫ��systickʱ����
    SysTick->LOAD=0xFFFFFF;
	SysTick->VAL=0xFFFFFF;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
}								    

//��ʱnus
//nusΪҪ��ʱ��us��.		    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	volatile u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;
	ticks=nus*fac_us;
	told=SysTick->VAL;
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)
                break;
		}  
	}; 
}
//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK��λΪHz,nms��λΪms
//��72M������,nms<=1864
//48M nms<=2796.2025
void delay_ms(u16 nms)
{	 		  	  
	delay_us((u32)(nms*1000));  	    
} 
