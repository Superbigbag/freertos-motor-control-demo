#ifndef __AD_H
#define __AD_H

extern uint16_t AD_Value;     // 外部声明一下，该变量既是AD.c文件的全局变量，又是外部可调用的变量，从而达到真正的全局。

void AD_Init(void);

void ADC_Switch(FunctionalState state);








#endif
