#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

struct led_state {
    uint8_t led_setting:1;
    uint8_t led_delay:1;
    uint8_t led_power:1;
    uint8_t led_connect:1;
    uint8_t led_running:1;
};

void normal_blink_led();
void sort_blink_led();
void quick_blink_led();
void long_blink_led();
void solid_led();