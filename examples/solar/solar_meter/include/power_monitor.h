#ifndef _POWER_MONITOR_H_
#define _POWER_MONITOR_H_

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "gpio_state_monitor.h"
#include "delay_monitor.h"

BaseType_t power_consume_measure_main_task();
BaseType_t power_panel_measure_main_task();

#ifdef __cplusplus
}
#endif

#endif