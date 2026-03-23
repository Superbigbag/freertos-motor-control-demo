#ifndef __Key_H
#define __Key_H

#include <stdint.h>
#include "FreeRTOS.h"



typedef enum {
    KEY_OFF = 0,
    KEY_ON
} KeyState_t;


extern volatile KeyState_t Key_switch_state;





void Key_Init(void);
void Key_Scan(void);



#endif
