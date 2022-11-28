#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../include/led_state_monitor.h"

#define QUICK_DELAY     100
#define SORT_DELAY      500
#define NORMAL_DELAY    1000
#define LONG_DELAY      2000

struct led_state led_status;

void running_normal_led()
{
    led_status.led_running = ~led_status.led_running;
    vTaskDelay(NORMAL_DELAY / portTICK_PERIOD_MS);
    return;
}

void quick_blink_led()
{
    vTaskDelay(QUICK_DELAY / portTICK_PERIOD_MS);
    return;
}

void sort_blink_led()
{
    vTaskDelay(SORT_DELAY / portTICK_PERIOD_MS);
    return;
}

void long_blink_led()
{
    vTaskDelay(LONG_DELAY / portTICK_PERIOD_MS);
    return;
}

void setting_up_led()
{
    return;
}

void normal_blink_led()
{
    running_normal_led();
}

void solid_led()
{
    return;
}

// static void blink_led(void)
// {
//     /* Set the GPIO level according to the state (LOW or HIGH)*/
//     gpio_set_level(BLINK_GPIO, s_led_state);
// }

// static void gpio_init(void)
// {
//     ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
//     gpio_reset_pin(BLINK_GPIO);
//     /* Set the GPIO as a push/pull output */
//     gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
// }

// void task_led_setting()
// {
//     blink_led();
//     quick_blink_led();
// }

// void task_led_running()
// {

// }