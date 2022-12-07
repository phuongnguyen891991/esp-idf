#ifndef _LCD_DISPLAY_H_
#define _LCD_DISPLAY_H_

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

struct lcdDisplay {
    int number;
    char *array;
};

BaseType_t lcd_display_main_task();

#endif