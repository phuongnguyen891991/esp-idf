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
#define LED_PANEL_GPIO 5

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

struct gpio_config wifi_led_tracking;
struct gpio_config panel_led_tracking;
struct gpio_config consume_led_tracking;

TaskHandle_t xTaskWifiSTate;
TaskHandle_t xTaskLedSTate;
TaskHandle_t xTaskLedPanel;
TaskHandle_t xTaskLedConsume;

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

    gpio_reset_pin(led->gpio);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(led->gpio, led->gpio_mode);
}

void init_wifi_led_tracking()
{
    wifi_led_tracking.gpio = GPIO_LED_WIFI;
    wifi_led_tracking.speed = (MODE_NORMAL_DELAY*2);
    wifi_led_tracking.gpio_mode = GPIO_MODE_OUTPUT;
    wifi_led_tracking.state = led_status.wifi_checking;
    // initialize_gpio(&wifi_led_tracking);
}

void init_power_panel_led()
{
    panel_led_tracking.gpio = GPIO_LED_PANEL;
    panel_led_tracking.speed = (MODE_NORMAL_DELAY*2);
    panel_led_tracking.gpio_mode = GPIO_MODE_OUTPUT;
    panel_led_tracking.state = led_status.panel_led_tracking;
    // initialize_gpio(&panel_led_tracking);
}

void init_power_consume_led()
{
    consume_led_tracking.gpio = GPIO_LED_CONSUME;
    consume_led_tracking.speed = (MODE_NORMAL_DELAY*2);
    consume_led_tracking.gpio_mode = GPIO_MODE_OUTPUT;
    consume_led_tracking.state = led_status.consume_led_tracking;
    // initialize_gpio(&consume_led_tracking);
}

void init_kitchen_light()
{
    kitchen_light.gpio = KITCHEN_LIGHT;
    kitchen_light.speed = (MODE_NORMAL_DELAY*2);
    kitchen_light.gpio_mode = GPIO_MODE_OUTPUT;
    kitchen_light.state = led_status.kitchen_checking;
    // initialize_gpio(&kitchen_light);
}

void init_exhausted_fan()
{
    exhausted_fan.gpio = EXHAUSTED_FAN;
    exhausted_fan.speed = (MODE_NORMAL_DELAY*2);
    exhausted_fan.gpio_mode = GPIO_MODE_OUTPUT;
    exhausted_fan.state = led_status.exhaust_checking;
    // initialize_gpio(&exhausted_fan);
}

void led_working_status()
{
    led_working.gpio = BLINK_GPIO;
    led_working.speed = (NORMAL_DELAY*2);
    led_working.gpio_mode = GPIO_MODE_OUTPUT;
    led_working.state = led_status.led_running;
}

void config_gpio_led_working_state()
{
    led_working_status();
    init_wifi_led_tracking();
    init_power_panel_led();
    init_power_consume_led();
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

void solid_led(struct gpio_config *led)
{
    if (led == NULL)
        return;
    ESP_LOGI(TAG, "GPIO LED (%d) solid %d", led->gpio, led->state);
    // blink_led(led);
    gpio_set_level(led->gpio, led->state);
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

void led_tracking(struct gpio_config *led, bool *led_type)
{
    if (led == NULL)
        return;

    initialize_gpio(led);
    while (1)
    {
        if (FALSE_STATE == *led_type)
        {
            solid_led(led);
            delay_msecond(MODE_NORMAL_DELAY);
        }
        else
            running_led(led);
    }
    return;
}

void led_state_wifi_tracking(void *para)
{
    struct gpio_config *led = (struct gpio_config*) para;

    if (para == NULL)
        return;

    led_tracking(led, &wifi_connection);
    return;
}

void led_state_panel_tracking(void *para)
{
    struct gpio_config *led = (struct gpio_config*) para;

    if (para == NULL)
        return;

    led_tracking(led, &panel_checking);
    return;
}

void led_state_consume_tracking(void *para)
{
    struct gpio_config *led = (struct gpio_config*) para;

    if (para == NULL)
        return;

    led_tracking(led, &consume_checking);
    return;
}

void deinit_task_led_state()
{
    if (xTaskLedSTate != NULL)
        vTaskDelete(xTaskLedSTate);

    if (xTaskWifiSTate != NULL)
        vTaskDelete(xTaskWifiSTate);

    if (xTaskLedConsume != NULL)
        vTaskDelete(xTaskLedConsume);

    if (xTaskLedPanel != NULL)
        vTaskDelete(xTaskLedPanel);
}

BaseType_t led_state_main_task()
{
    BaseType_t xReturn;
    memset(&led_status, 0, sizeof(struct led_checking));
    config_gpio_led_working_state();

    xReturn = xTaskCreate(led_state_main_loop, "Task blink LED", 2 * BUF_SIZE_TASK, &led_working, 1, &xTaskLedSTate);
    if(xReturn == pdPASS)
    {
        /* The task was created.  Use the task's handle to delete the task. */
        ESP_LOGI(TAG, "The task of led state was created ");
    }

    xReturn = xTaskCreate(led_state_wifi_tracking, "Wi-Fi tracking led", 2 * BUF_SIZE_TASK, &wifi_led_tracking, 1, &xTaskWifiSTate);
    if(xReturn == pdPASS)
    {
        /* The task was created.  Use the task's handle to delete the task. */
        ESP_LOGI(TAG, "The task of Wi-Fi tracking was created ");
    }

    xReturn = xTaskCreate(led_state_panel_tracking, "Panel tracking led", 2 * BUF_SIZE_TASK, &panel_led_tracking, 1, &xTaskLedPanel);
    if(xReturn == pdPASS)
    {
        /* The task was created.  Use the task's handle to delete the task. */
        ESP_LOGI(TAG, "The task of Panel led tracking was created ");
    }

    xReturn = xTaskCreate(led_state_consume_tracking, "Consume tracking led", 2 * BUF_SIZE_TASK, &consume_led_tracking, 1, &xTaskLedConsume);
    if(xReturn == pdPASS)
    {
        /* The task was created.  Use the task's handle to delete the task. */
        ESP_LOGI(TAG, "The task of Consume led tracking was created ");
    }

    return xReturn;
}
