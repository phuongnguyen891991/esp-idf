
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


#define RET_ERR 1
#define RET_OK  0

#define TRUE_STATE  1
#define FALSE_STATE 0

// define GPIO number for using in this project
#define GPIO_LED_PANEL      (15)
#define GPIO_LED_CONSUME    (17)
#define GPIO_LED_WIFI       (19)

#define GPIO_MEA_VOL_PANEL  (20)
#define GPIO_MEA_CUR_PANEL  (21)
#define GPIO_MEA_VOL_CONSUM (22)
#define GPIO_MEA_CUR_CONSUM (23)

#define BUF_SIZE_TASK 1024

typedef struct power_measure {
    uint8_t vol;
    uint8_t current;
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


static QueueHandle_t power_manage_queue;
static QueueHandle_t power_consume_queue;

static uint16_t g_power_panel_mea;
static uint16_t g_power_consume_mea;

static TaskHandle_t xTaskLedSTate;
static TaskHandle_t xTaskPanelMea;
static TaskHandle_t xTaskLedPanel;
static TaskHandle_t xTaskLedConsume;
static TaskHandle_t xTaskPowerConsume;


#ifdef __cplusplus
}
#endif

#endif  /* GENERIC_DEFINE_H */