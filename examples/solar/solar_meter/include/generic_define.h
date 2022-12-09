
#ifndef _GENERIC_DEFINE_H_
#define _GENERIC_DEFINE_H_

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "esp_heap_caps_init.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"


#define RET_ERR 1
#define RET_OK  0

#define TRUE_STATE  1
#define FALSE_STATE 0

// define GPIO number for using in this project
#define GPIO_LED_PANEL      GPIO_NUM_17
#define GPIO_LED_CONSUME    GPIO_NUM_18
#define GPIO_LED_WIFI       GPIO_NUM_5

#define KITCHEN_LIGHT   GPIO_NUM_25
#define EXHAUSTED_FAN   GPIO_NUM_26

#define GPIO_MEA_VOL_PANEL  ADC1_CHANNEL_0 // GPIO_NUM_36
#define GPIO_MEA_CUR_PANEL  ADC1_CHANNEL_3 // GPIO_NUM_39
#define GPIO_MEA_VOL_CONSUM ADC1_CHANNEL_6 // GPIO_NUM_34
#define GPIO_MEA_CUR_CONSUM ADC1_CHANNEL_7 // GPIO_NUM_35

#define BUF_SIZE_TASK 1024
#define MAX_QUEUE_SIZE  10
#define PANEL_TYPE  1
#define CONSUME_TYPE 2
#define INTERNAL_CHECK  100

typedef struct power_storage {
    uint32_t power;
    uint8_t type;
};

typedef struct power_measure {
    uint32_t vol;
    uint32_t current;
    uint32_t power;
    uint8_t type;
};

enum delay_mode {
    MODE_QUICK_DELAY     = 100,
    MODE_SORT_DELAY      = 500,
    MODE_NORMAL_DELAY    = 1000,
    MODE_LONG_DELAY      = 2000,
    MODE_UNKNOW          = 0
};

enum led_indicate {
    LED_RUNNING_STATE       = 2,
    LED_WIFI_STATE          = 3,
    LED_POWER_PANEL_STATE   = 4,
    LED_POWER_CONS_STATE    = 5,
    LED_UNKNOWN_STATE       = 0
};

typedef struct gpio_config {
    gpio_num_t  gpio;
    uint32_t    speed;
    gpio_mode_t gpio_mode;
    uint8_t     state;
};

extern QueueHandle_t power_panel_queue;
extern QueueHandle_t power_consume_queue;

extern TaskHandle_t xTaskPanelMea;
extern TaskHandle_t xTaskLedPanel;
extern TaskHandle_t xTaskConsumeMea;
extern TaskHandle_t xTaskLedConsume;
extern TaskHandle_t xTaskLedSTate;

extern TaskHandle_t xTaskLcdDisplayPanel;
extern TaskHandle_t xTaskLcdDisplayConsume;

// static TaskHandle_t xTaskMeaVolPanel;
// static TaskHandle_t xTaskMeaCurPanel;
// static TaskHandle_t xTaskMeaVolConsume;
// static TaskHandle_t xTaskMeaCurConsume;

typedef struct power_in_cache {
    uint32_t previous;
    uint32_t now;
    uint32_t kp_power; // the units in kilo power
};

extern struct gpio_config kitchen_light;
extern struct gpio_config exhausted_fan;
extern struct gpio_config led_power_panel;
extern struct gpio_config led_power_consume;

extern bool wifi_connection;
extern bool panel_checking;
extern bool consume_checking;

#ifdef __cplusplus
}
#endif

#endif  /* GENERIC_DEFINE_H */