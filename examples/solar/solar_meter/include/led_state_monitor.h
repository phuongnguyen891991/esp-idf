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
    uint8_t led_power_panel:1;
    uint8_t led_power_consume:1;
    uint8_t led_connect:1;
    uint8_t led_running:1;
};

typedef struct gpio_config {
    gpio_num_t  gpio;
    uint8_t     speed;
    gpio_mode_t gpio_mode;
    uint8_t     state;
};

static struct led_checking led_status;
static struct gpio_config led_running;
static struct gpio_config led_power_panel;
static struct gpio_config led_power_consume;

void led_state_main_loop(void *para);
BaseType_t led_state_main_task();

#ifdef __cplusplus
}
#endif

#endif