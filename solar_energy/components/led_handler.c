/* Blink Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "led_handler.h"

#define BLINK_GPIO 2

// this function is using for normal state
void normal_led(void *arg) {
    static uint8_t s_led_state = 0;
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        s_led_state = !s_led_state;
        gpio_set_level(BLINK_GPIO, s_led_state);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// Will hsave solid led if hang services
void hang_service(void *arg) {
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(BLINK_GPIO, 1);
}

void on_off_led(uint8_t gpio_num, uint8_t state, uint8_t delay) {
    gpio_set_level(gpio_num, state);
    vTaskDelay(delay / portTICK_PERIOD_MS);
    gpio_set_level(gpio_num, !state);
    vTaskDelay(delay / portTICK_PERIOD_MS);
}

// if have no wifi; it is blink: on-off-on
void no_wifi_led(void *arg) {
    static uint8_t s_led_state = 0;
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        uint8_t i;
        for (i = 0; i < 3; i++)
            on_off_led(BLINK_GPIO, s_led_state, 100);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void connecting_wifi_led(void *arg) {
    static uint8_t s_led_state = 0;
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1)
        on_off_led(BLINK_GPIO, s_led_state, 200);
}