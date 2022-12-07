#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "gpio_state_monitor.h"
#include "delay_monitor.h"
#include "generic_define.h"
#include "lcd2004_i2c.h"

//function to received data from queue, then show in LCD
static const char *TAG = "LCD Display";

TaskHandle_t xTaskLcdDisplayPanel;
TaskHandle_t xTaskLcdDisplayConsume;


#define STICK_TO_WAIT  portMAX_DELAY

void get_power_panel_from_queue()
{
    // struct power_storage data[MAX_QUEUE_SIZE] = {0};
    struct power_storage data;
    BaseType_t xStatus;

    while (1)
    {
        if (power_panel_queue != 0)
        {
            xStatus = xQueueReceive(power_panel_queue, (void*)&data, (TickType_t)STICK_TO_WAIT);
            if (xStatus == pdPASS)
            {
                ESP_LOGI(TAG, "power of queue chanel: %.2d, %.2d ", (int)data.power, (int)data.type);
            }
            else
                ESP_LOGI(TAG, "power of queue chanel empty ");
        }
        delay_msecond(MODE_NORMAL_DELAY);
    }
    return;
}

void get_power_consume_from_queue()
{
    // struct power_storage data[MAX_QUEUE_SIZE] = {0};
    struct power_storage data;
    BaseType_t xStatus;

    while(1)
    {
        if (power_consume_queue != 0)
        {
            xStatus = xQueueReceive(power_consume_queue, (void*)&data, (TickType_t)STICK_TO_WAIT);
            if (xStatus == pdPASS)
            {
                ESP_LOGI(TAG, "power of queue consume: %.2d, %.2d ", (int)data.power, (int)data.type);
                // for (int i = 0; i < MAX_QUEUE_SIZE; i++)
                // {
                //     ESP_LOGI(TAG, "power of queue consume: %.2d, %.2d ", (int)data[i].power, (int)data[i].type);
                // }
            }
            else
                ESP_LOGI(TAG, "power of queue consume empty ");
        }
        delay_msecond(MODE_NORMAL_DELAY);
    }
    return;
}

void lcd_display()
{
    return;
}

void lcd_deinit()
{
    if (xTaskLcdDisplayPanel != NULL)
        vTaskDelete(xTaskLcdDisplayPanel);

    if (xTaskLcdDisplayConsume != NULL)
        vTaskDelete(xTaskLcdDisplayConsume);
}

uint8_t lcd_display_init()
{
    BaseType_t xReturn;

    xReturn = xTaskCreate(get_power_panel_from_queue, "get power panel from queue", 2 * BUF_SIZE_TASK, NULL, 1, &xTaskLcdDisplayPanel);
    if(xReturn == pdPASS)
    {
        /* The task was created.  Use the task's handle to delete the task. */
        ESP_LOGI(TAG, "The task of LCD display power panel was created ");
    }

    xReturn = xTaskCreate(get_power_consume_from_queue, "get power consume from queue", 2 * BUF_SIZE_TASK, NULL, 1, &xTaskLcdDisplayConsume);
    if(xReturn == pdPASS)
    {
        /* The task was created.  Use the task's handle to delete the task. */
        ESP_LOGI(TAG, "The task of LCD display power consume was created ");
    }
    return xReturn;
}

BaseType_t lcd_display_main_task()
{
    BaseType_t xReturn;

    xReturn = lcd_display_init();
    if(xReturn == pdPASS)
    {
        /* The task was created.  Use the task's handle to delete the task. */
        ESP_LOGI(TAG, "The task of LCD display was created ");
    }
    return xReturn;
}