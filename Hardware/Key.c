#include "stm32f10x.h"                  // Device header
#include "Key.h"  


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

volatile KeyState_t Key_switch_state = KEY_OFF;

void Key_Init(void)                     // LED端口初始化
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU ;       // 上拉输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);    // 初始化后的输入端口默认为高电平，因为是上拉

}






void Key_Scan(void)                  // 按键代码优化：之前的会在20ms内翻转多次，因为按一下可能是80ms。
									 // 现在利用下降沿进行判断，即上20ms是高电平和这20ms是低电平，才算按一下。
{
    static uint8_t last = 1;     
    uint8_t now = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);

   
    if ((last == 1) && (now == 0))
    {
        Key_switch_state = (Key_switch_state == KEY_OFF) ? KEY_ON : KEY_OFF;
    }

    last = now;
}



