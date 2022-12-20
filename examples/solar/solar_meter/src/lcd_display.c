#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "gpio_state_monitor.h"
#include "delay_monitor.h"
#include "generic_define.h"
#include "esp_system.h"
#include "rom/uart.h"
#include "delay_monitor.h"
#include "smbus.h"
#include "i2c-lcd1602.h"
#include "lcd_display.h"

static const char *TAG = "LCD Display";

// LCD2004
#define LCD_NUM_ROWS               4
#define LCD_NUM_COLUMNS            20
#define LCD_NUM_VISIBLE_COLUMNS    20
#define CONFIG_LCD1602_I2C_ADDRESS 0x27

// Undefine USE_STDIN if no stdin is available (e.g. no USB UART) - a fixed delay will occur instead of a wait for a keypress.
#define USE_STDIN  1
//#undef USE_STDIN
#define portTICK_RATE_MS    portTICK_PERIOD_MS

#define I2C_MASTER_NUM           I2C_NUM_0
#define I2C_MASTER_TX_BUF_LEN    0                     // disabled
#define I2C_MASTER_RX_BUF_LEN    0                     // disabled
#define I2C_MASTER_FREQ_HZ       100000
#define I2C_MASTER_SDA_IO        GPIO_NUM_21 //CONFIG_I2C_MASTER_SDA
#define I2C_MASTER_SCL_IO        GPIO_NUM_22 //CONFIG_I2C_MASTER_SCL

#define LCD_MAX_STRING  LCD_NUM_COLUMNS

TaskHandle_t xTaskLcdDisplayPanel;
TaskHandle_t xTaskLcdDisplayConsume;

SemaphoreHandle_t lcdDisplaySema = NULL;

#define STICK_TO_WAIT  portMAX_DELAY

typedef struct _lcd_data_dp {
    struct power_storage pPanel;
    struct power_storage pConsume;
    char ipv4[IPV4_STR_SIZE];
} lcd_data_dp;

lcd_data_dp lcd_dp;
bool blink_heart = true;

static void i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;  // GY-2561 provides 10kΩ pullups
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;  // GY-2561 provides 10kΩ pullups
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_MASTER_RX_BUF_LEN,
                       I2C_MASTER_TX_BUF_LEN, 0);
}

void get_power_panel_from_queue()
{
    struct power_storage data;
    BaseType_t xStatus;

    while (1)
    {
        if (power_panel_queue != 0)
        {
            xStatus = xQueueReceive(power_panel_queue, (void*)&data, (TickType_t)STICK_TO_WAIT);
            if (xStatus == pdPASS)
            {
                ESP_LOGI(TAG, "power of queue chanel: %.2d, %.2d ", (int)data.power, (int)data.type);
                lcd_dp.pPanel.power = data.power;
                lcd_dp.pPanel.vol = data.vol;
                lcd_dp.pPanel.current = data.current;
            }
            else
                ESP_LOGI(TAG, "power of queue chanel empty ");
        }
        delay_msecond(MODE_NORMAL_DELAY);
    }
    return;
}

void get_power_consume_from_queue()
{
    struct power_storage data;
    BaseType_t xStatus;

    while(1)
    {
        if (power_consume_queue != 0)
        {
            xStatus = xQueueReceive(power_consume_queue, (void*)&data, (TickType_t)STICK_TO_WAIT);
            if (xStatus == pdPASS)
            {
                ESP_LOGI(TAG, "power of queue consume: %.2d, %.2d ", (int)data.power, (int)data.type);
                lcd_dp.pConsume.power = data.power;
                lcd_dp.pConsume.vol = data.vol;
                lcd_dp.pConsume.current = data.current;
            }
            else
                ESP_LOGI(TAG, "power of queue consume empty ");
        }
        delay_msecond(MODE_NORMAL_DELAY);
    }
    return;
}

void lcd_display()
{
    return;
}

void lcd_deinit()
{
    if (xTaskLcdDisplayPanel != NULL)
        vTaskDelete(xTaskLcdDisplayPanel);

    if (xTaskLcdDisplayConsume != NULL)
        vTaskDelete(xTaskLcdDisplayConsume);
}

uint8_t lcd_display_init()
{
    BaseType_t xReturn;

    xReturn = xTaskCreate(get_power_panel_from_queue, "get power panel from queue", BUF_SIZE_TASK, NULL, 1, &xTaskLcdDisplayPanel);
    if(xReturn != pdPASS)
        ESP_LOGI(TAG, "The task of LCD display power panel was created ");

    xReturn = xTaskCreate(get_power_consume_from_queue, "get power consume from queue", BUF_SIZE_TASK, NULL, 1, &xTaskLcdDisplayConsume);
    if(xReturn != pdPASS)
        ESP_LOGI(TAG, "The task of LCD display power consume was created ");

    return xReturn;
}

void lcd_show_power(void *vPara)
{
    char lcd_string[LCD_MAX_STRING];

    i2c_lcd1602_info_t *lcd_info = (i2c_lcd1602_info_t *)vPara;
    if (NULL == lcd_info)
        return;

    i2c_lcd1602_move_cursor(lcd_info, 0, 0);
    memset(lcd_string, 0, sizeof(lcd_string));
    sprintf(lcd_string, "Vp:%2ld |Vc:%2ld |", lcd_dp.pPanel.vol, lcd_dp.pConsume.vol);
    i2c_lcd1602_write_string(lcd_info, lcd_string);

    i2c_lcd1602_move_cursor(lcd_info, 0, 1);
    memset(lcd_string, 0, sizeof(lcd_string));
    sprintf(lcd_string, "Ip:%2ld |Ic:%2ld |", lcd_dp.pPanel.current, lcd_dp.pConsume.current);
    i2c_lcd1602_write_string(lcd_info, lcd_string);

    i2c_lcd1602_move_cursor(lcd_info, 0, 2);
    memset(lcd_string, 0, sizeof(lcd_string));
    sprintf(lcd_string, "Pp:%3ld|Pc:%3ld|", lcd_dp.pPanel.power, lcd_dp.pConsume.power);
    i2c_lcd1602_write_string(lcd_info, lcd_string);

    i2c_lcd1602_move_cursor(lcd_info, 0, 3);
    memset(lcd_string, 0, sizeof(lcd_string));
    strncpy(lcd_dp.ipv4, g_ipv4_str, IPV4_STR_SIZE-1);
    sprintf(lcd_string, "ip%s", lcd_dp.ipv4);
    i2c_lcd1602_write_string(lcd_info, lcd_string);
}

void lcd_blink_heart_state(void *vPara)
{
    uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};

    i2c_lcd1602_info_t *lcd_info = (i2c_lcd1602_info_t *)vPara;
    if (NULL == lcd_info)
        return;

    i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_0, heart);
    if (blink_heart == true)
    {
        i2c_lcd1602_move_cursor(lcd_info, 19, 0);
        i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_0);
        i2c_lcd1602_move_cursor(lcd_info, 19, 1);
        i2c_lcd1602_write_char(lcd_info, ' ');
    }
    else
    {
        i2c_lcd1602_move_cursor(lcd_info, 19, 0);
        i2c_lcd1602_write_char(lcd_info, ' ');
        i2c_lcd1602_move_cursor(lcd_info, 19, 1);
        i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_0);
    }
    blink_heart = !blink_heart;
}

void lcd_2004_task(void *pvParameter)
{
    i2c_port_t i2c_num = I2C_MASTER_NUM;
    uint8_t address = CONFIG_LCD1602_I2C_ADDRESS;

    lcdDisplaySema = xSemaphoreCreateBinary();
    if (NULL == lcdDisplaySema)
        return;

    // Set up I2C
    i2c_master_init();
    // Set up the SMBus
    smbus_info_t * smbus_info = smbus_malloc();
    ESP_ERROR_CHECK(smbus_init(smbus_info, i2c_num, address));
    ESP_ERROR_CHECK(smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS));

    i2c_lcd1602_info_t * lcd_info = i2c_lcd1602_malloc();
    ESP_ERROR_CHECK(i2c_lcd1602_init(lcd_info, smbus_info, true,
                                     LCD_NUM_ROWS, LCD_NUM_COLUMNS, LCD_NUM_VISIBLE_COLUMNS));

    i2c_lcd1602_set_backlight(lcd_info, true);
    delay_msecond(MODE_QUICK_DELAY);

    i2c_lcd1602_move_cursor(lcd_info, 0, 0);
    i2c_lcd1602_write_string(lcd_info, "starting .....");
    delay_msecond(MODE_LONG_DELAY);
    i2c_lcd1602_clear(lcd_info);

    while (true)
    {
        if (NULL != lcdDisplaySema)
        {
            if (pdPASS == xSemaphoreTake(lcdDisplaySema, MODE_QUICK_DELAY))
            {
                lcd_show_power(lcd_info);
                lcd_blink_heart_state(lcd_info);
            }
            xSemaphoreGive(lcdDisplaySema);
        }
        delay_msecond(MODE_NORMAL_DELAY);
    }

    return;
}

BaseType_t lcd_display_main_task()
{
    BaseType_t xReturn;

    xReturn = xTaskCreate(&lcd_2004_task, "lcd2004_task", 2 * BUF_SIZE_TASK, NULL, 5, NULL);
    if(xReturn != pdPASS)
        ESP_LOGI(TAG, "The task of LCD display power panel was created failed");

    return xReturn;
}