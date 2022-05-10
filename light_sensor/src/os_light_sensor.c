
/**   
 ***************************************************************
 * @file    mylib/gps/src/os_zedf9p.c    
 * @author  Lewis Ainslie - s4485827  
 * @date    12/04/2021   
 * @brief   HCI hal source file    
 ***************************************************************
 * EXTERNAL FUNCTIONS
***************************************************************
* os_init_zedf9p () - initialise gps receive therad
***************************************************************
 */

#include "os_light_sensor.h"
#include "hal_light_sensor.h"
#include "registry.h"
#include "peripheral.h"
#include "os_peripheral.h"


#define STACKSIZE 256
#define PRIORITY 7
#define UPDATE_PERIOD 1000

K_THREAD_STACK_DEFINE(light_sensor_receive_stack_area, STACKSIZE);

static k_tid_t light_sensor_tid;
static struct k_thread light_sensor_thread;

#define REGISTRY_DEPTH 10


/* 
 * Function used to start gps receiver thread. 
 * Returns 0 for success
 * 
 */
int os_init_light_sensor() {
    int num_sensors;

    if (hal_init_light_sensors(&num_sensors))
        return -1;

    peripheral_fetch *tracker = k_calloc(1, sizeof(struct peripheral_fetch));

    tracker->hal_driver = hal_get_light_sensors_reading;
    tracker->num_peripherals = num_sensors;
    tracker->peripheral_type = LIGHT_SENSOR;


    

    printk("passing light sensor %d \n", num_sensors);

    light_sensor_tid = k_thread_create(&light_sensor_thread, light_sensor_receive_stack_area,\
                        K_THREAD_STACK_SIZEOF(light_sensor_receive_stack_area),\
                        peripheral_tracking,\
                        tracker, NULL, NULL,\
                        PRIORITY, 0, K_NO_WAIT);
    return 0;
}



