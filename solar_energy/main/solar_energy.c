/* ADC1 Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "adc_handler.h"
#include "led_handler.h"

TaskHandle_t adcHandle;
TaskHandle_t ledHandle;

void solar_energy_service()
{
    BaseType_t xReturn;

    xReturn = xTaskCreate(adc_interface_service, "adc_handler_task", 1024*100, NULL, 5, &adcHandle);
    if (pdPASS == xReturn) {
        printf("Already start adc handler ! \n");
    } else {
        vTaskDelete(adcHandle);
    }

    xReturn = xTaskCreate(blink_led, "blink led", 512, NULL, 5, &ledHandle);
    if (pdPASS == xReturn) {
        printf("Already start led handler ! \n");
    } else {
        vTaskDelete(ledHandle);
    }
}

void app_main(void) {
    solar_energy_service();
}
