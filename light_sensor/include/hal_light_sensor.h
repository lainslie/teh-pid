/**   
 ***************************************************************
 * @file    apps/thesis/include/main.h   
 * @author  Lewis Ainslie - s4485827  
 * @date    08/11/2021   
 * @brief   Main header file for includes    
 ***************************************************************
 */


#ifndef __HAL_LIGHT_SENSOR_H__
#define __HAL_LIGHT_SENSOR_H__

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <init.h>
#include <drivers/adc.h>
#include <drivers/uart.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "peripheral.h"
#include "os_light_sensor.h"

#define ADC_RESOLUTION		12
#define ADC_GAIN			ADC_GAIN_1
#define ADC_REFERENCE		ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME	ADC_ACQ_TIME_DEFAULT
#define BUFFER_SIZE			10

struct light_sensor {
    int channel;
    int sensor_id;
    struct device *adc_device;
    peripheral_datablock *data;
};

peripheral_datablock **hal_get_light_sensors_reading();


int hal_init_light_sensors(int *num_devices);

#endif