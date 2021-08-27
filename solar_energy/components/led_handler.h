#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

void normal_led(void *pvParameter);
void hang_service(void *arg);
void no_wifi_led(void *arg);
void connecting_wifi_led(void *arg);