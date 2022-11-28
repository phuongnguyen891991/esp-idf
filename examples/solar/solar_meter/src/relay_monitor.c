#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "../include/relay_monitor.h"


void delay_input_second(int seconds)
{
    vTaskDelay(seconds / portTICK_PERIOD_MS);
    return;
}

void delay_in_msecond(int mseconds)
{
    vTaskDelay(mseconds / portTICK_PERIOD_MS);
    return;
}

void gpio_delay(int gpio_num, int seconds)
{
    return;
}
