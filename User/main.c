#include "stm32f10x.h"                  // Device header
#include "Motor.h"
#include "AD.h"
#include "Key.h"
#include "MPU6050.h"
#include "Serial.h"

#include <math.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



QueueHandle_t xQueue_01;


volatile uint8_t  Speed;
float a_norm;
float theta;
float theta_deg;



void ADFunction(void * param)                     // 具体任务函数1
{
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	
	while(1)
	{
		if(Key_switch_state == KEY_ON)
		{
			ADC_Switch(ENABLE);
			
//	  	  	xQueueSend(xQueue_02, &theta_deg, 0);     		  // 常规写法，适合于不丢数据的队列
			xQueueOverwrite(xQueue_01, &AD_Value); 			  // 始终覆盖成最新值写法,队列长度一般为1
			
		}
		
		else
		{
			ADC_Switch(DISABLE);
			AD_Value = 0;
		}
		
		vTaskDelay(10);
	}
	
}




void MotorFunction(void * param)                     // 具体任务函数2
{
		
	uint16_t ReceiveAD;
	
	
	while(1)
	{
		xQueueReceive(xQueue_01, &ReceiveAD, 0);
		
		
		if(Key_switch_state == KEY_ON)
		{
			if((ReceiveAD*100/4096) > 50){Speed = 20;Motor_SetSpeed(Speed);} 
			else{Speed = 50;Motor_SetSpeed(Speed);}
		}
		else
		{
			Motor_SetSpeed(0);
			Speed = 0;
		}
	
	}
		
}



void KeyFunction(void * param)                     // 具体任务函数3
{
		
	while(1)
	{
		Key_Scan();
		
		vTaskDelay(pdMS_TO_TICKS(10)); 
		
	}
}


void MPU6050Function(void * param)
{
	int16_t AX, AY, AZ;
	
	
	while(1)
	{
		if(Key_switch_state == KEY_ON)
		{
			MPU6050_Switch(ENABLE);
			
			MPU6050_GetData(&AX, &AY, &AZ);
			
			float fax = (float)AX, fay = (float)AY, faz = (float)AZ;
			a_norm = sqrtf(fax*fax + fay*fay + faz*faz);
			theta = acosf(AZ / a_norm);                 // 与Z轴夹角
			theta_deg = theta * 57.2957795f;

		}
		else 
		{
			
			MPU6050_Switch(DISABLE);     				// 休眠成功，寄存器值不更新了，维持在最后一次数值
			
			theta_deg = 0;
			
			
		}
			
		vTaskDelay(pdMS_TO_TICKS(10));
		
		
	}
}


void SerialFunction(void * param)                     
{
	
	
	while(1)
	{
		
		
		Serial_SendString("*********************");
		Serial_SendString("\r\n");
		Serial_SendString("Key = ");
		Serial_SendNumber(Key_switch_state, 1);
		Serial_SendString("\r\n");
		
		Serial_SendString("AD = ");
		Serial_SendNumber(AD_Value, 4);                      // 使用全局变量传值，也可以使用队列传值
		Serial_SendString("\r\n");
		
		Serial_SendString("Speed = ");
		Serial_SendNumber(Speed, 2);
		Serial_SendString("%\r\n");
		
		Serial_SendString("angle = ");
		Serial_SendFloat(theta_deg);
		Serial_SendString("\r\n");
		Serial_SendString("\r\n");
		
		
		
		
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}




int main(void)
{ 

	TaskHandle_t  xADHandle;
	
	
	
	AD_Init();
	Motor_Init();
	Key_Init();
	MPU6050_Init();
	Serial_Init();
	

	xQueue_01 = xQueueCreate(1, sizeof( uint16_t ));
	
	
	xTaskCreate(ADFunction, "AD", 256, NULL, 1, &xADHandle);             // 创建任务
	xTaskCreate(MotorFunction, "Motor", 256, NULL, 1, NULL); 
	xTaskCreate(KeyFunction, "Key", 100, NULL, 1, NULL); 
	xTaskCreate(MPU6050Function, "MPU6050", 256, NULL, 1, NULL); 
	xTaskCreate(SerialFunction, "Serial", 256, NULL, 1, NULL); 
	
	vTaskStartScheduler();
	
	
	while (1)
	{
		
		
	} 
}






