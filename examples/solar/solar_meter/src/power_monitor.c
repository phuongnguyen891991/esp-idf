#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "power_monitor.h"
#include "generic_define.h"
#include "gpio_state_monitor.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF        1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES       64          //Multisampling

#define MAX_QUEUE           10
#define ADC_EXAMPLE_ATTEN   ADC_ATTEN_DB_11

static const char *TAG = "Power";

QueueHandle_t power_panel_queue;
QueueHandle_t power_consume_queue;

TaskHandle_t xTaskPanelMea;
TaskHandle_t xTaskConsumeMea;

SemaphoreHandle_t xSemaphore_pn = NULL;
SemaphoreHandle_t xSemaphore_cs = NULL;

struct power_storage kp_panel_storage;
struct power_storage kp_consume_storage;

struct power_in_cache kp_panel;
struct power_in_cache kp_consume;

bool panel_checking;
bool consume_checking;

uint32_t getMillis(void)
{
    struct timeval tv = { 0 };
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

uint8_t queue_power_panel_init()
{
    memset(&kp_panel_storage, 0, sizeof(struct power_storage));

    power_panel_queue = xQueueCreate(MAX_QUEUE_SIZE, sizeof(struct power_storage));
    if (power_panel_queue == 0)
        ESP_LOGI(TAG, "Failed to create queue for data panel!");

    xSemaphore_pn = xSemaphoreCreateBinary();
    if (NULL == xSemaphore_pn)
        return RET_ERR;

    return RET_OK;
}

uint8_t queue_power_consume_init()
{
    memset(&kp_consume_storage, 0, sizeof(struct power_storage));

    power_consume_queue = xQueueCreate(MAX_QUEUE_SIZE, sizeof(struct power_storage));
    if (power_consume_queue == 0)
        ESP_LOGI(TAG, "Failed to create queue for data consume!");

    xSemaphore_cs = xSemaphoreCreateBinary();
    if (NULL == xSemaphore_cs)
        return RET_ERR;

    return RET_OK;
}

uint8_t push_data_to_queue(struct power_storage *power)
{
    BaseType_t xStatus;

    if (power == NULL)
        return RET_ERR;

    if (PANEL_TYPE == power->type)
    {
        if(NULL != xSemaphore_pn)
        {
            if(xSemaphoreTake(xSemaphore_pn, MODE_QUICK_DELAY) == pdPASS)
            {
                xStatus = xQueueSendToBack(power_panel_queue, (void*)power, (TickType_t)0);
                if (xStatus != pdPASS)
                    ESP_LOGI(TAG, "Failed to send panel data to queue!");
            }
            xSemaphoreGive(xSemaphore_pn);
        }
    }
    else if(CONSUME_TYPE == power->type)
    {
        if (NULL != xSemaphore_cs)
        {
            if (xSemaphoreTake(xSemaphore_cs, MODE_QUICK_DELAY) == pdPASS)
            {
                xStatus = xQueueSendToBack(power_consume_queue, (void*)power, (TickType_t)0);
                if (xStatus != pdPASS)
                    ESP_LOGI(TAG, "Failed to send consume data to queue!");
            }
            xSemaphoreGive(xSemaphore_cs);
        }
    }
    else
        return RET_ERR;

    return RET_OK;
}

uint8_t read_power(uint32_t *measure, adc_channel_t channel)
{
    uint32_t voltage = 0;
    static esp_adc_cal_characteristics_t adc_chars;

    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
    ESP_ERROR_CHECK(adc1_config_channel_atten(channel, ADC_EXAMPLE_ATTEN));

    uint32_t adc_reading = 0;

    for (int i = 0; i < NO_OF_SAMPLES; i++)
        adc_reading += adc1_get_raw((adc1_channel_t)channel);

    adc_reading /= NO_OF_SAMPLES;
    //Convert adc_reading to voltage in mV
    voltage = esp_adc_cal_raw_to_voltage(adc_reading, &adc_chars);
    ESP_LOGI(TAG, "Channel: %d Raw: %d\tVoltage: %d mV", channel, (int)adc_reading, (int)voltage);
    *measure = voltage;

    return RET_OK;
}

uint8_t power_analyze(struct power_measure *measure, struct power_in_cache *power)
{
    uint32_t capacity = 0;

    if ((power == NULL) || (measure == NULL))
        return RET_ERR;

    power->now = getMillis();
    if ((power->now - power->previous) >= INTERNAL_CHECK)
    {
        if ((measure->vol == 0) || (measure->current == 0))
            return RET_ERR;

        // units of vol and Current: Vol and Ampe
        capacity = measure->vol * measure->current;
        power->kp_power = power->kp_power + (capacity/(1000*3600));
    }
    power->previous = power->now;
    return RET_OK;
}

uint8_t power_calculation(struct power_measure *measure)
{
    uint8_t status;

    if (measure == NULL)
        return RET_ERR;

    if (PANEL_TYPE == measure->type)
    {
        status = power_analyze(measure, &kp_panel);
        if (RET_OK != status)
        {
            panel_checking = FALSE_STATE;
            return RET_ERR;
        }

        kp_panel_storage.power = kp_panel.kp_power;
        kp_panel_storage.vol = measure->vol;
        kp_panel_storage.current = measure->current;
        kp_panel_storage.type = PANEL_TYPE;
        push_data_to_queue(&kp_panel_storage);
        panel_checking = TRUE_STATE;
    }
    else if (CONSUME_TYPE == measure->type)
    {
        status = power_analyze(measure, &kp_consume);
        if (RET_OK != status)
        {
            consume_checking = FALSE_STATE;
            return RET_ERR;
        }

        kp_consume_storage.power = kp_consume.kp_power;
        kp_consume_storage.vol = measure->vol;
        kp_consume_storage.current = measure->current;
        kp_consume_storage.type = CONSUME_TYPE;
        push_data_to_queue(&kp_consume_storage);
        consume_checking = TRUE_STATE;
    }
    else
    {
        ESP_LOGI(TAG, "Not known type to push data to queue");
        return RET_ERR;
    }

    return RET_OK;
}

void power_panel_measure_main_loop()
{
    while(1)
    {
        struct power_measure measure;
        uint8_t status = 0;

        status = read_power(&measure.vol, ADC1_CHANNEL_0);
        if (status != RET_OK)
            measure.vol = 0;

        status = read_power(&measure.current, ADC1_CHANNEL_3);
        if (status != RET_OK)
            measure.current = 0;

        measure.type = PANEL_TYPE;
        status = power_calculation(&measure);
        if (status != RET_OK)
            delay_msecond(MODE_NORMAL_DELAY);

        delay_msecond(MODE_NORMAL_DELAY);
    }
}

void power_consume_measure_main_loop()
{
    while(1)
    {
        struct power_measure measure;
        uint8_t status = 0;

        status = read_power(&measure.vol, ADC1_CHANNEL_6);
        if (status != RET_OK)
            measure.vol = 0;

        status = read_power(&measure.current, ADC1_CHANNEL_7);
        if (status != RET_OK)
            measure.current = 0;

        measure.type = CONSUME_TYPE;
        status = power_calculation(&measure);
        if (status != RET_OK)
            delay_msecond(MODE_NORMAL_DELAY);

        delay_msecond(MODE_NORMAL_DELAY);
    }
}

BaseType_t power_measure_init_task()
{
    BaseType_t xReturn;

    queue_power_panel_init();

    xReturn = xTaskCreate(power_panel_measure_main_loop, "task measure power from panel", 2 * BUF_SIZE_TASK, NULL, 1, &xTaskPanelMea);
    if(xReturn != pdPASS)
        ESP_LOGI(TAG, "The task of power panel measure init failed");

    return xReturn;
}

BaseType_t power_consume_init_task()
{
    BaseType_t xReturn;

    queue_power_consume_init();

    xReturn = xTaskCreate(power_consume_measure_main_loop, "measure power from comsume", 2 * BUF_SIZE_TASK, NULL, 1, &xTaskConsumeMea);
    if(xReturn != pdPASS)
        ESP_LOGI(TAG, "The task of power consume measure init failed ");

    return xReturn;
}

void power_deinit_task()
{
    if (xTaskPanelMea != NULL)
    {
        vTaskDelete(xTaskPanelMea);
        xTaskPanelMea = NULL;
    }

    if (xTaskConsumeMea != NULL)
    {
        vTaskDelete(xTaskConsumeMea);
        xTaskConsumeMea = NULL;
    }

    if (xSemaphore_pn != NULL)
    {
        vSemaphoreDelete(xSemaphore_pn);
        xSemaphore_pn = NULL;
    }
    if (xSemaphore_cs != NULL)
    {
        vSemaphoreDelete(xSemaphore_cs);
        xSemaphore_cs = NULL;
    }
}

BaseType_t power_panel_measure_main_task()
{
    BaseType_t xReturn;

    xReturn = power_measure_init_task();
    if(xReturn != pdPASS)
        ESP_LOGI(TAG, "The main task of power panel was created ");

    return xReturn;
}

BaseType_t power_consume_measure_main_task()
{
    BaseType_t xReturn;

    xReturn = power_consume_init_task();
    if(xReturn != pdPASS)
        ESP_LOGI(TAG, "The main task of power consume was created ");

    return xReturn;
}
