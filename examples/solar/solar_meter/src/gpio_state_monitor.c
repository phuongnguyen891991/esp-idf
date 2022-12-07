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

#define BLINK_GPIO 2
static const char *TAG = "LED";

#define DELAY_IN_SEC    2
#define QUICK_DELAY     100
#define SORT_DELAY      500
#define NORMAL_DELAY    1000
#define LONG_DELAY      2000

struct led_checking led_status;
struct gpio_config led_working;
struct gpio_config kitchen_light;
struct gpio_config exhausted_fan;

TaskHandle_t xTaskLedSTate;


static void blink_led(struct gpio_config *led)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(led->gpio, led->state);
}

void config_gpio_led_working_state()
{
    led_working.gpio = BLINK_GPIO;
    led_working.speed = (NORMAL_DELAY*2);
    led_working.gpio_mode = GPIO_MODE_OUTPUT;
    led_working.state = led_status.led_running;
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

    gpio_reset_pin(led->gpio);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(led->gpio, led->gpio_mode);
}

void init_kitchen_light()
{
    kitchen_light.gpio = KITCHEN_LIGHT;
    kitchen_light.speed = 0;
    kitchen_light.gpio_mode = GPIO_MODE_OUTPUT;
    kitchen_light.state = FALSE_STATE;
    initialize_gpio(&kitchen_light);
}

void init_exhausted_fan()
{
    exhausted_fan.gpio = EXHAUSTED_FAN;
    exhausted_fan.speed = 0;
    exhausted_fan.gpio_mode = GPIO_MODE_OUTPUT;
    exhausted_fan.state = FALSE_STATE;
    initialize_gpio(&exhausted_fan);
}

void kitchen_light_func(bool request)
{
    init_kitchen_light();
    gpio_set_level(kitchen_light.gpio, request);

    return;
}

void exhausted_fan_func(bool request)
{
    init_exhausted_fan();
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
    ESP_LOGI(TAG, "GPIO LED (%d) status : %s", led->gpio, led->state == 255 ? "ON" : "OFF");
    led->state = ~(led->state);
    delay_msecond(led->speed);
    return;
}

void led_state_main_loop(void *para)
{
    memset(&led_status, 0, sizeof(struct led_checking));
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

void deinit_task_led_state()
{
    if (xTaskLedSTate != NULL)
        vTaskDelete(xTaskLedSTate);
}

BaseType_t led_state_main_task()
{
    BaseType_t xReturn;
    config_gpio_led_working_state();

    xReturn = xTaskCreate(led_state_main_loop, "Task blink LED", 2 * BUF_SIZE_TASK, &led_working, 1, &xTaskLedSTate);
    if(xReturn == pdPASS)
    {
        /* The task was created.  Use the task's handle to delete the task. */
        ESP_LOGI(TAG, "The task of led state was created ");
    }

    return xReturn;
}
