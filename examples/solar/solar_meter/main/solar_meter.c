/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
// #include "led_strip.h"
#include "sdkconfig.h"
#include "gpio_state_monitor.h"
#include "power_monitor.h"
#include "lcd_display.h"
#include "wifi_config.h"

void app_main(void)
{
    led_state_main_task();
    power_consume_measure_main_task();
    power_panel_measure_main_task();
    lcd_display_main_task();
    wifi_config();
}
