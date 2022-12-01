#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "power_monitor.h"
#include "generic_define.h"
#include "led_state_monitor.h"

#define MAX_QUEUE   10
static const char *TAG = "power";

uint32_t getMillis(void)
{
    struct timeval tv = { 0 };
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

uint8_t queue_power_init()
{
    memset(&kp_panel, 0, sizeof(struct power_in_queue));
    memset(&kp_consume, 0, sizeof(struct power_in_queue));

    power_panel_queue = xQueueCreate(MAX_QUEUE_SIZE, sizeof(struct power_measure));
    if (power_panel_queue == 0)
    {
        ESP_LOGI(TAG, "Failed to create queue !");
    }

    power_consume_queue = xQueueCreate(MAX_QUEUE_SIZE, sizeof(struct power_measure));
    if (power_consume_queue == 0)
    {
        ESP_LOGI(TAG, "Failed to create queue !");
    }

    return RET_OK;
}

uint8_t push_data_to_queue(uint32_t *power, uint8_t type)
{
    if (power == NULL)
        return RET_ERR;

    if (PANEL_TYPE == type)
        xQueueSend(power_panel_queue, (void*)power, (TickType_t)0);
    else if(PANEL_TYPE == type)
        xQueueSend(power_consume_queue, (void*)power, (TickType_t)0);
    else
        return RET_ERR;

    return RET_OK;
}

uint8_t calculate_power(struct power_measure *measure, struct power_in_queue *power)
{
    uint32_t capacity = 0;

    if ((power == NULL) || (measure == NULL))
        return RET_ERR;

    power->now = getMillis();
    if ((power->now - power->previous) >= INTERNAL_CHECK)
    {
        if ((measure->vol == 0) || (measure->current == 0))
            return RET_ERR;
        // units of vol and Current: Vol and Ampe
        capacity = measure->vol * measure->current;
        power->kp_power = power->kp_power + (capacity/(1000*3600));
    }
    power->previous = power->now;
    return RET_OK;
}

uint8_t power_panel_calculate(struct power_measure *measure)
{
    uint8_t status;

    if (measure == NULL)
        return RET_ERR;

    status = calculate_power(measure, &kp_panel);
    if (RET_OK != status)
        return RET_ERR;

    push_data_to_queue(kp_panel.kp_power, PANEL_TYPE);

    // store the value into queue
    return RET_OK;
}

uint8_t power_consume_calculate(struct power_measure *measure)
{
    uint8_t status;
    if (measure == NULL)
        return RET_ERR;

    status = calculate_power(measure, &kp_consume);
    if (RET_OK != status)
        return RET_ERR;

    push_data_to_queue(kp_consume.kp_power, CONSUME_TYPE);
    // store the value into queue
    return RET_OK;
}

void power_panel_measure_main_loop()
{
    while (1)
    {
        // read and calculate data
        ESP_LOGI(TAG, "Power panel measurement !");
        // push data into queue
        // display LCD
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

void init_gpio_power_panel()
{
    led_power_panel.gpio = GPIO_LED_PANEL;
    led_power_panel.speed = MODE_NORMAL_DELAY;
    led_power_panel.gpio_mode = GPIO_MODE_OUTPUT;
    led_power_panel.state = led_status.led_power_panel;
}

void init_gpio_power_consume()
{
    led_power_consume.gpio = GPIO_LED_CONSUME;
    led_power_consume.speed = MODE_NORMAL_DELAY;
    led_power_consume.gpio_mode = GPIO_MODE_OUTPUT;
    led_power_consume.state = led_status.led_power_consume;
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

    init_gpio_power_panel();
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

    init_gpio_power_consume();
    xReturn = xTaskCreate(led_state_main_loop, "", 2 * BUF_SIZE_TASK, &led_power_consume, 0, &xTaskLedConsume);
    if(xReturn == pdPASS)
    {
        /* The task was created.  Use the task's handle to delete the task. */
        ESP_LOGI(TAG, "turn on led to tracking power from panel ");
    }
    return xReturn;
}

void power_deinit_task()
{
    if (xTaskLedConsume != NULL)
    {
        vTaskDelete(xTaskLedConsume);
    }

    if (xTaskLedPanel != NULL)
    {
        vTaskDelete(xTaskLedPanel);
    }
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
