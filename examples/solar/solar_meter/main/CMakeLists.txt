

list(APPEND srcs
        "solar_meter.c"
        "../src/relay_monitor.c"
        "../src/led_state_monitor.c"
        "../src/power_monitor.c"
        "../src/lcd_display.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                                    "../src"
                                    "../include")
