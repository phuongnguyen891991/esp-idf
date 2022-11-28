#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


struct gpioDelay{
    int gpio_num;
    int seconds;
};

void delay_input_second(int seconds);
void delay_in_msecond(int mseconds);
void gpio_delay(int gpio_num, int seconds);