
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

#define GPIO_MEA_VOL_PANEL  (32)
#define GPIO_MEA_CUR_PANEL  (33)
#define GPIO_MEA_VOL_CONSUM (34)
#define GPIO_MEA_CUR_CONSUM (35)

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
    uint8_t vol;
    uint8_t current;
    uint16_t power;
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


static QueueHandle_t power_panel_queue;
static QueueHandle_t power_consume_queue;

static uint16_t g_power_panel_mea;
static uint16_t g_power_consume_mea;


static TaskHandle_t xTaskPanelMea;
static TaskHandle_t xTaskLedPanel;
static TaskHandle_t xTaskLedConsume;

static TaskHandle_t xTaskLcdDisplayPanel;
static TaskHandle_t xTaskLcdDisplayConsume;

// typedef struct TaskHandleLed {
    static TaskHandle_t TaskLedSTate;
    static TaskHandle_t TaskLedPanel;
    static TaskHandle_t TaskLedConsume;
// };

// typedef struct TaskHandleSensor {
    static TaskHandle_t xTaskMeaVolPanel;
    static TaskHandle_t xTaskMeaCurPanel;
    static TaskHandle_t xTaskMeaVolConsume;
    static TaskHandle_t xTaskMeaCurConsume;
// };

typedef struct power_in_cache {
    uint32_t previous;
    uint32_t now;
    uint32_t kp_power; // the units in kilo power
};

static struct power_in_cache kp_panel;
static struct power_in_cache kp_consume;

static struct power_storage kp_panel_storage;
static struct power_storage kp_consume_storage;

#ifdef __cplusplus
}
#endif

#endif  /* GENERIC_DEFINE_H */