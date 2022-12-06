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

void get_power_panel_from_queue()
{
    struct power_storage data;

    memset(&data, 0, sizeof(struct power_storage));
    if (power_panel_queue != 0)
    {
        if (xQueueReceive(power_panel_queue, &(data), (TickType_t)10))
        {
            ESP_LOGI(TAG, "power of queue chanel: %.2d, %.2d ", (int)data.power, (int)data.type);
        }
    }

    return;
}

void get_power_consume_from_queue()
{
    struct power_storage data;

    memset(&data, 0, sizeof(struct power_storage));
    if (power_consume_queue != 0)
    {
        if (xQueueReceive(power_consume_queue, &(data), (TickType_t)10))
        {
            ESP_LOGI(TAG, "power of queue consume: %.2d, %.2d ", (int)data.power, (int)data.type);
        }
    }

    return;
}

void lcd_display()
{
    return;
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