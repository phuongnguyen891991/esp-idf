#ifndef _LED_STATE_MONITOR_H_
#define _LED_STATE_MONITOR_H_

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

typedef struct led_checking {
    uint8_t led_setting:1;
    uint8_t led_delay:1;
    uint8_t led_power:1;
    uint8_t led_connect:1;
    uint8_t led_running:1;
};

static void configure_led(gpio_num_t gpio_num, gpio_mode_t mode);
void running_led(gpio_num_t gpio_num, uint8_t speed_mode);
BaseType_t led_state_main_task();

#ifdef __cplusplus
}
#endif

#endif