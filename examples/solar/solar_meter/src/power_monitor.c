#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "power_monitor.h"
#include "generic_define.h"
#include "led_state_monitor.h"

#define MAX_QUEUE   10
static const char *TAG = "power";


uint8_t queue_init()
{
    return RET_OK;
}

uint8_t push_data_to_queue(uint8_t *input)
{
    QueueHandle_t queue_handle;

    queue_handle = xQueueCreate(10, sizeof(*input));
    return RET_OK;
}

uint8_t power_panel_calculate(uint8_t vol, uint8_t current)
{
    uint8_t power = 0;
    if ((vol == 0) || (current == 0))
    {
        return RET_ERR;
    }

    power = vol * current;
    // store the value into queue
    return RET_OK;
}

uint8_t power_consume_calculate(uint8_t vol, uint8_t current)
{
    uint8_t power = 0;
    if ((vol == 0) || (current == 0))
    {
        return RET_ERR;
    }

    power = vol * current;
    // store the value into queue
    return RET_OK;
}

void power_panel_measure_main_loop()
{
    // should init queue
    // configure_led(gpio, mode);
    while (1)
    {
        // running_led(gpio, speed);
        ESP_LOGI(TAG, "Power panel measurement !");
        delay_second(1);
    }
    return;
}

void power_consume_measure_main_loop()
{
    // should init queue
    while (1)
    {
        // running_led(gpio, speed);
        ESP_LOGI(TAG, "Power consume measurement !");
        delay_second(1);
    }
    return;
}

BaseType_t power_measure_init_task()
{
    BaseType_t xReturn;

    xReturn = xTaskCreate(power_panel_measure_main_loop, "task measure power from panel", 2 * BUF_SIZE_TASK, NULL, 1, &xTaskPanelMea);
    if(xReturn == pdPASS)
    {
        /* The task was created.  Use the task's handle to delete the task. */
        ESP_LOGI(TAG, "The task of power panel was created ");
        // vTaskDelete(xTaskLedSTate);
        // Because of successful task, we need to create task blink led status
    }

    led_power_panel.gpio = GPIO_LED_PANEL;
    led_power_panel.speed = MODE_NORMAL_DELAY;
    led_power_panel.gpio_mode = GPIO_MODE_OUTPUT;
    led_power_panel.state = led_status.led_power_panel;

    xReturn = xTaskCreate(led_state_main_loop, "", 2 * BUF_SIZE_TASK, &led_power_panel, 0, &xTaskLedPanel);
    if(xReturn == pdPASS)
    {
        /* The task was created.  Use the task's handle to delete the task. */
        ESP_LOGI(TAG, "turn on led to tracking power from panel ");
    }

    return xReturn;
}

BaseType_t power_consume_init_task()
{
    BaseType_t xReturn;

    xReturn = xTaskCreate(power_consume_measure_main_loop, "measure power from panel", 2 * BUF_SIZE_TASK, NULL, 1, &xTaskPanelMea);
    if(xReturn == pdPASS)
    {
        /* The task was created.  Use the task's handle to delete the task. */
        ESP_LOGI(TAG, "The task of power panel was created ");
    }

    led_power_consume.gpio = GPIO_LED_CONSUME;
    led_power_consume.speed = MODE_NORMAL_DELAY;
    led_power_consume.gpio_mode = GPIO_MODE_OUTPUT;
    led_power_consume.state = led_status.led_power_consume;

    xReturn = xTaskCreate(led_state_main_loop, "", 2 * BUF_SIZE_TASK, &led_power_consume, 0, &xTaskLedConsume);
    if(xReturn == pdPASS)
    {
        /* The task was created.  Use the task's handle to delete the task. */
        ESP_LOGI(TAG, "turn on led to tracking power from panel ");
    }
    return xReturn;
}

BaseType_t power_panel_measure_main_task()
{
    BaseType_t xReturn;

    xReturn = power_measure_init_task();
    if(xReturn == pdPASS)
    {
        ESP_LOGI(TAG, "The task of power panel was created ");
    }
    return xReturn;
}

BaseType_t power_consume_measure_main_task()
{
    BaseType_t xReturn;

    xReturn = power_consume_init_task();
    if(xReturn == pdPASS)
    {
        /* The task was created.  Use the task's handle to delete the task. */
        ESP_LOGI(TAG, "The task of power consume was created ");
    }
    return xReturn;
}
