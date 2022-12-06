/*
    Th file will handle all function of delay include:
    - Delay in ms
    - Delay in second
    - Delay in minutes
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "relay_monitor.h"

void delay_minutes(int number)
{
    vTaskDelay((number*60*1000) / portTICK_PERIOD_MS);
    return;
}

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