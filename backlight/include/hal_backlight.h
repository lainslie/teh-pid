#ifndef _HAL_BACKLIGHT_H
#define _HAL_BACKLIGHT_H


#include <stdlib.h>
#include <string.h>
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include "peripheral.h"


peripheral_datablock **hal_get_backlight_statuses();
int hal_init_backlights(int *num_devices);


#endif