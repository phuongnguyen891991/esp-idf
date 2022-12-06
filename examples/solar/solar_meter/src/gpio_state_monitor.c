#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "gpio_state_monitor.h"
#include "delay_monitor.h"
#include "generic_define.h"

#define BLINK_GPIO 2
static const char *TAG = "Led";

#define QUICK_DELAY     100
#define SORT_DELAY      500
#define NORMAL_DELAY    1000
#define LONG_DELAY      2000

static void blink_led(struct gpio_config *led)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(led->gpio, led->state);
}

/*
* para:
* gpio_num  : the gpio number on the board, ex: BLINK_GPIO
* mode      : input or output mode, ex: GPIO_MODE_OUTPUT
*/
void initialize_gpio(struct gpio_config *led)
{
    if (led == NULL)
        return;

    ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
    gpio_reset_pin(led->gpio);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(led->gpio, led->gpio_mode);
}

void kitchen_light(bool request)
{
    struct gpio_config kitchen_light;
    kitchen_light.gpio = KITCHEN_LIGHT;
    kitchen_light.gpio_mode = GPIO_MODE_OUTPUT;

    initialize_gpio(&kitchen_light);
    gpio_set_level(kitchen_light.gpio, request);

    return;
}

void exhausted_fan(bool request)
{
    struct gpio_config exhausted_fan;
    exhausted_fan.gpio = EXHAUSTED_FAN;
    exhausted_fan.gpio_mode = GPIO_MODE_OUTPUT;

    initialize_gpio(&exhausted_fan);
    gpio_set_level(exhausted_fan.gpio, request);

    return;
}

/*
* para:
* gpio_num      : the gpio number on the board, ex: BLINK_GPIO
* speed_mode    : the mode of relay, ex: NORMAL_DELAY
*/
void running_led(struct gpio_config *led)
{
    if (led == NULL)
        return;

    blink_led(led);
    ESP_LOGI(TAG, "GPIO LED (%d) status : %d", led->gpio, led->state);
    led->state = ~(led->state);
    delay_msecond(led->speed);
    return;
}

void led_state_main_loop(void *para)
{
    struct gpio_config *led = (struct gpio_config*) para;
    if (para == NULL)
        return;

    initialize_gpio(led);
    while(1)
    {
        running_led(led);
    }
    return;
}

BaseType_t led_state_main_task()
{
    BaseType_t xReturn;

    led_running.gpio = BLINK_GPIO;
    led_running.speed = NORMAL_DELAY;
    led_running.gpio_mode = GPIO_MODE_OUTPUT;
    led_running.state = led_status.led_running;

    xReturn = xTaskCreate(led_state_main_loop, "task blink led", 2 * BUF_SIZE_TASK, &led_running, 1, &TaskLedSTate);
    if(xReturn == pdPASS)
    {
        /* The task was created.  Use the task's handle to delete the task. */
        ESP_LOGI(TAG, "The task of led state was created ");
        // vTaskDelete(xTaskLedSTate);
    }
    return xReturn;
}
