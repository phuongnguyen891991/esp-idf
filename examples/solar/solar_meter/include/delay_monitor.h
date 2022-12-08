#ifndef _DELAY_MONITOR_H_
#define _DELAY_MONITOR_H_

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


struct gpioDelay{
    int gpio_num;
    int seconds;
};

void delay_second(int seconds);
void delay_msecond(int mseconds);
void gpio_delay(int gpio_num, int seconds);
void relay_main_task();

#ifdef __cplusplus
}
#endif

#endif