#include "FreeRTOS.h"
#include "task.h"

static void led_task(void *argument)
{
    while (1)
    {
        // TODO: 翻转LED
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void MX_FREERTOS_Init(void)
{
    xTaskCreate(led_task, "led", 128, NULL, 2, NULL);
}
