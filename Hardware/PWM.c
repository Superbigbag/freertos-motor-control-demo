#include "stm32f10x.h"                  // Device header


void PWM_Init(void)
{	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           // 复用推挽输出，不接数据寄存器，接其他外设
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period =  100 - 1;     //ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;    //PSC 该项只会影响PWM频率,不影响占空比
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);              // 用来给结构体赋初始值，以应对下面只需非全部的一些结构体赋值的情况
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse =0;                     // CRR 寄存器的值,频率为1000hz，占空比50%，分辨率为1%
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);               //  分辨率 = 1/（ARR+1），占空比 = CRR /（ARR+1）,频率 = CLK_PSC /（PSC+1）/（ARR+1）
	
	TIM_Cmd(TIM2, ENABLE);                                 // 最后一步，准备就绪 启动定时器,代表都打通了，开始输出信号了

}

void PWM_SetCompare3(uint16_t Compare)
{
	TIM_SetCompare3(TIM2, Compare);                    // 该函数是设置CRR的值,进而改变占空比

}

