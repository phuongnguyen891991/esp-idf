idf_component_register(SRCS "adc_handler.c"
                            "led_handler.c"
                            INCLUDE_DIRS "."
                            REQUIRES "esp_adc_cal")
