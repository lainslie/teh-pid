#ifndef _HAL_FAN_H
#define _HAL_FAN_H


#include <stdlib.h>
#include <string.h>
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/pwm.h>
#include "peripheral.h"


peripheral_datablock **hal_get_fan_feedbacks();
int hal_init_fans(int *num_devices);

int hal_set_fan_speed(int fan_num, int duty_cycle);

int hal_set_fans(int fan_num, int duty_cycle);

#endif