#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_state_monitor.h"
#include "relay_monitor.h"
#include "generic_define.h"

#define BLINK_GPIO 2
static const char *TAG = "Led";

#define QUICK_DELAY     100
#define SORT_DELAY      500
#define NORMAL_DELAY    1000
#define LONG_DELAY      2000

struct led_checking led_status;

static void blink_led(gpio_num_t gpio_num)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(gpio_num, led_status.led_running);
}

/*
* para:
* gpio_num  : the gpio number on the board, ex: BLINK_GPIO
* mode      : input or output mode, ex: GPIO_MODE_OUTPUT
*/
static void configure_led(gpio_num_t gpio_num, gpio_mode_t mode)
{
    ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
    gpio_reset_pin(gpio_num);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(gpio_num, mode);
}

/*
* para:
* gpio_num      : the gpio number on the board, ex: BLINK_GPIO
* speed_mode    : the mode of relay, ex: NORMAL_DELAY
*/
void running_led(gpio_num_t gpio_num, uint8_t speed_mode)
{
    blink_led(gpio_num);
    ESP_LOGI(TAG, "GPIO LED status : %d", led_status.led_running);
    led_status.led_running = ~led_status.led_running;
    delay_msecond(speed_mode);
    return;
}

void led_state_main_loop()
{
    configure_led(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1)
    {
        running_led(BLINK_GPIO, (uint8_t) NORMAL_DELAY);
    }
    return;
}

BaseType_t led_state_main_task()
{
    BaseType_t xReturn;
    xReturn = xTaskCreate(led_state_main_loop, "task blink led", 2*1024, NULL, 1, &xTaskLedSTate);
    if(xReturn == pdPASS)
    {
        /* The task was created.  Use the task's handle to delete the task. */
        ESP_LOGI(TAG, "The task of led state was created ");
        // vTaskDelete(xTaskLedSTate);
    }
    return xReturn;
}