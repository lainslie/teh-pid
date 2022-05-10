#ifndef _HAL_DOOR_SENSOR_H
#define _HAL_DOOR_SENSOR_H


#include <stdlib.h>
#include <string.h>
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include "peripheral.h"


peripheral_datablock **hal_get_door_statuses();
int hal_init_door_sensors(int *num_devices);


#endif