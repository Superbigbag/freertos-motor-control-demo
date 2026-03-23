#ifndef __LED_H
#define __LED_H

//#define LED1 GPIO_Pin_1
//#define LED2 GPIO_Pin_2
//#define ON 1
//#define OFF 0

typedef enum{LED1=GPIO_Pin_1, LED2=GPIO_Pin_2} LEDx_t;   // 类型转换定义：将枚举改成自己定的名字
typedef enum{ON=1,OFF=0} LED_STATE_t;                    // 枚举函数：限定设置的范围，也可理解为宏定义的集合

void LED_Init(void);

void LED_SETTING(LEDx_t LEDx , LED_STATE_t LED_STATE);

void LED1_Turn(void);
void LED2_Turn(void);



void LED1_ON(void);

void LED1_OFF(void);

void LED2_ON(void);

void LED2_OFF(void);



#endif
