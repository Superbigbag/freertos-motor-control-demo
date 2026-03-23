#include "stm32f10x.h"                  // Device header
#include "LED.h"

void LED_Init(void)    // LED端口初始化
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;           // 推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;    // 使用输出端口PA1,PA2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);                     // 初始化后的输出端口默认低电平
	
	GPIO_SetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2);              // 这里改为高电平
	
}


void LED_SETTING(LEDx_t LEDx , LED_STATE_t LED_STATE)       // LEDx 开关函数
{
	if(LEDx==LED1 && LED_STATE==ON)                         // LED1 部分
		GPIO_ResetBits(GPIOA, LED1);
	else if(LEDx==LED1 && LED_STATE==OFF)
		GPIO_SetBits(GPIOA, LED1);
	
	if(LEDx==LED2 && LED_STATE==ON)                        // LED2 部分
		GPIO_ResetBits(GPIOA, LED2);
	else if(LEDx==LED2 && LED_STATE==OFF)
		GPIO_SetBits(GPIOA, LED2);
}


void LED1_Turn(void)                                    // LED1 翻转函数
{
	if( GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1)==0 )
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
	else 
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	
}
void LED2_Turn(void)                                     // LED2 翻转函数
{
	if( GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_2)==0 )
		GPIO_SetBits(GPIOA, GPIO_Pin_2);
	else
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);	
	
}


//void LED1_ON(void)                                 // LED1,LED2开关函数（简单版）
//{
//	GPIO_ResetBits(GPIOA, GPIO_Pin_1);

//}

//void LED1_OFF(void)
//{
//	GPIO_SetBits(GPIOA, GPIO_Pin_1);

//}

//void LED2_ON(void)
//{
//	GPIO_ResetBits(GPIOA, GPIO_Pin_2);

//}

//void LED2_OFF(void)
//{
//	GPIO_SetBits(GPIOA, GPIO_Pin_2);

//}
