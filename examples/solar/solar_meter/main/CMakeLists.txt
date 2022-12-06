list(APPEND srcs
        "solar_meter.c"
        "../src/delay_monitor.c"
        "../src/gpio_state_monitor.c"
        "../src/power_monitor.c"
        "../src/lcd_display.c"
        "../src/lcd2004_i2c.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                                    "../src"
                                    "../include")
