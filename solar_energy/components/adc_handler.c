/* ADC1 Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "adc_handler.h"

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

static const adc_channel_t volt = ADC_CHANNEL_6;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
static const adc_channel_t current = ADC1_CHANNEL_7; // GPIO35 is input of current sensor

static const adc_atten_t atten = ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;

QueueHandle_t xQueue;
uint32_t g_volt = 0;
uint32_t g_curr = 0;

typedef enum{
    VOLT_SENDER,
    CURR_SENDER,
    NONE
} Sender_t;

typedef struct {
    uint32_t ucValue;
    Sender_t Sender;
} Data_t;

static void check_efuse(void) {
    //Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }
    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
}

static void print_char_val_type(esp_adc_cal_value_t val_type) {
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}

//return value:
//  ESP_FAIL
//  ESP_OK
static uint8_t initialize_adc(uint8_t gpio, esp_adc_cal_characteristics_t *adc_char) {
    //Check if Two Point or Vref are burned into eFuse
    check_efuse();

    //Configure ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(width);
        adc1_config_channel_atten(gpio, atten);
    }
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_char);
    print_char_val_type(val_type);
    return ESP_OK;
}

void volt_measure_service(void *arg) {
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
    Data_t volt_sender;

    volt_sender.Sender = VOLT_SENDER;
    static esp_adc_cal_characteristics_t *adc_chars;
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    if (ESP_FAIL == initialize_adc(volt, adc_chars)) {
        free(adc_chars);
        adc_chars = NULL;
        return;
    }

    //Continuously sample ADC1
    while (1) {
        uint32_t adc_reading = 0;
        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            if (unit == ADC_UNIT_1)
                adc_reading += adc1_get_raw((adc1_channel_t)volt);
        }
        adc_reading /= NO_OF_SAMPLES;
        //Convert adc_reading to voltage in mV
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        volt_sender.ucValue = voltage;

        xStatus = xQueueSendToBack(xQueue, &volt_sender, xTicksToWait);
        if (pdPASS != xStatus) {
            printf("Failed to send volt data to queue \n");
            continue;
        }
        // printf("Raw (volt sensor): %d\tVoltage: %dmV\n", adc_reading, voltage);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void current_measure_service(void *arg) {
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
    Data_t curr_sender;

    curr_sender.Sender = CURR_SENDER;
    static esp_adc_cal_characteristics_t *adc_chars;
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    if (ESP_FAIL == initialize_adc(volt, adc_chars)) {
        free(adc_chars);
        adc_chars = NULL;
        return;
    }

    //Continuously sample ADC1
    while (1) {
        uint32_t adc_reading = 0;
        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            if (unit == ADC_UNIT_1)
                adc_reading += adc1_get_raw((adc1_channel_t)current);
        }
        adc_reading /= NO_OF_SAMPLES;
        //Convert adc_reading to voltage in mV
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        curr_sender.ucValue = voltage;
        xStatus = xQueueSendToBack(xQueue, &curr_sender, xTicksToWait);
        if (pdPASS != xStatus) {
            printf("Failed to current data to queue \n");
            continue;
        }
        // printf("Raw (current sensor): %d\tVoltage: %dmV\n", adc_reading, voltage);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void xQueue_received(void *arg) {
    BaseType_t xStatus;
    Data_t xReceived;
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );

    for (;;) {
        xStatus = xQueueReceive(xQueue, &xReceived, xTicksToWait);
        if (pdPASS == xStatus) {
            if (xReceived.Sender == VOLT_SENDER) {
                g_volt = xReceived.ucValue;
                printf("Queue volt value is: %d \n", xReceived.ucValue);
            } else if (xReceived.Sender == CURR_SENDER) {
                printf("Queue curr value is: %d \n", xReceived.ucValue);
                g_curr = xReceived.ucValue;
            } else {
                printf("Not handler for now");
            }
            printf("Power consum: %d \n", (g_volt*g_curr));
        }
    }
}

void calculate_power_consum(void *arg) {
    BaseType_t xQueueStatus;

    xQueue = xQueueCreate(3, sizeof(Data_t));
    if (xQueue != NULL) {
        xTaskCreate(volt_measure_service, "Vol measure", 2048, NULL, 2, NULL);
        xTaskCreate(current_measure_service, "curr measure", 2048, NULL, 2, NULL);
        xTaskCreate(xQueue_received, "Receiver", 4096, NULL, 1, NULL);
    } else {
        printf("Failed to create queue");
    }
}