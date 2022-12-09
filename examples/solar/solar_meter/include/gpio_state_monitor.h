#ifndef _GPIO_STATE_MONITOR_H_
#define _GPIO_STATE_MONITOR_H_

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "generic_define.h"

typedef struct led_checking {
    uint8_t led_setting:1;
    uint8_t led_delay:1;
    uint8_t panel_led_tracking:1;
    uint8_t consume_led_tracking:1;
    uint8_t led_connect:1;
    uint8_t led_running:1;
    uint8_t wifi_checking:1;
    uint8_t kitchen_checking:1;
    uint8_t exhaust_checking:1;
};

extern struct led_checking led_status;
extern struct gpio_config led_working;
extern struct gpio_config led_power_panel;
extern struct gpio_config led_power_consume;

void led_state_main_loop(void *para);
BaseType_t led_state_main_task();

#ifdef __cplusplus
}
#endif

#endif