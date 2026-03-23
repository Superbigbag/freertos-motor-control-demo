#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

uint8_t Serial_TxPacket[4];
uint8_t Serial_RxPacket[4];
uint8_t Serial_RxFlag;

void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength =  USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
	
	
}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}


void Serial_SendArray(uint8_t *Array, uint16_t Length)       // 定义个指针,填进去数组的首地址，内部由指针完成发送
{
	uint16_t i;
	for(i = 0; i < Length; i++)
	{
		Serial_SendByte(Array[i]);                        // 指针可以用 [] 运算符，指针的 Array[i] 等效于 *(Array + i)。和数组差不多。
	
	}

}



void Serial_SendString(char *String)  
{
	uint8_t i;
	for(i = 0; String[i] !=0; i++)
	{
		Serial_SendByte(String[i]);                        // 指针可以用 [] 运算符，指针的 Array[i] 等效于 *(Array + i)。和数组差不多。
	
	}

}

void Serial_SendFloat(float value)
{
    char buf[32];
    sprintf(buf, "%.2f", value);  						 // 例如 12.34
    Serial_SendString(buf);
}



uint32_t Serial_Pow(uint32_t X,uint32_t Y)              // 该函数代表X^Y
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
		
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length) 
{
	uint8_t i;
	for(i = 0; i < Length; i++)
	{
		Serial_SendByte(Number / Serial_Pow(10,Length-i-1)%10+ '0');                     
	 
	}

}


int fputc(int ch,FILE *f)          // printf的移植方法1
{

	Serial_SendByte(ch);
	return ch;

}




void Serial_Printf(char *format,...)    //  printf的移植方法3
{
	char  String[100];
	va_list arg;
	va_start(arg,format);
	vsprintf(String,format,arg);
	va_end(arg);
	Serial_SendString(String);


}




void Serial_SendPacket(void)
{
	Serial_SendByte(0xFF);
	Serial_SendArray(Serial_TxPacket, 4);
	Serial_SendByte(0xFE);

}


uint8_t  Serial_GetRxFlag(void)
{
	if(Serial_RxFlag == 1)
	{	
		Serial_RxFlag = 0;
		return 1;
	}
	
	return 0;
}






void USART1_IRQHandler(void)                                // 在中断里接收 数据包 的状态机程序
{
	static uint8_t RxState = 0;	                            // 静态变量只会初始化一次，第二次直接忽略这行
	static uint8_t pRxPacket = 0;	                          

	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)     // 这里需要加判断语句，因为USART1_IRQHandler是USART1整体的中断函数
															// 加个判断，判断是不是想要的中断位，因为也有可能是别的位
	{
		uint8_t RxData  = USART_ReceiveData(USART1);
		if(RxState == 0)
		{
			if(RxData == 0xFF)
			{
				RxState = 1;
				pRxPacket = 0;
			}
		
		}
		
		else if(RxState == 1)
		{
			Serial_RxPacket[pRxPacket] = RxData;
			pRxPacket ++;
			if(pRxPacket >= 4)
			{
				RxState = 2;
				
			}
		}
		
		else if(RxState == 2)
		{
			if(RxData == 0xFE)
			{
				RxState = 0;
				Serial_RxFlag = 1;
			}
		
		}
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		
	}
	
	
}












