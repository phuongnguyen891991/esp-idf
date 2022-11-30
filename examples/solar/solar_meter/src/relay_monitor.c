#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "relay_monitor.h"

void delay_second(int seconds)
{
    vTaskDelay((seconds*1000) / portTICK_PERIOD_MS);
    return;
}

void delay_msecond(int mseconds)
{
    vTaskDelay(mseconds / portTICK_PERIOD_MS);
    return;
}

void gpio_delay(int gpio_num, int seconds)
{
    return;
}

void relay_main_task()
{
    return;
}