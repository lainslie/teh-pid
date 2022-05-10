/**   
 ***************************************************************
 * @file    mylib/log/include/cli_log.h    
 * @author  Lewis Ainslie - s4485827  
 * @date    08/03/2021   
 * @brief   LED cli header file    
 ***************************************************************
* EXTERNAL FUNCTIONS
***************************************************************
* log_cli_init () - Initialise cli function for logging
***************************************************************
 */
#ifndef _PERIPHERAL_H
#define _PERIPHERAL_H

#include <stdlib.h>
#include <stdint.h>


#define GET_DEVICE(node_id, prop) device_get_binding(DT_LABEL(DT_PROP(node_id, prop))),
#define GET_CHANNEL(node_id, prop) DT_PROP(node_id, prop),

enum peripheral_access_type {CHANNEL, PIN};

struct peripheral_datablock {
    char status[5];
    int64_t time;
    char *information;
} typedef peripheral_datablock;

struct peripheral_device {
    enum peripheral_access_type access_type;

    union {
        int channel;
        int pin;
    } peripheral_access;

    struct device *device;
    peripheral_datablock *data;
} typedef peripheral_device;

typedef enum {
    LIGHT_SENSOR,
    FAN,
    DOOR,
    BACKLIGHT
} subsystem_type;



typedef struct peripheral_fetch {
    subsystem_type peripheral_type;
    int num_peripherals;
    peripheral_datablock **(*hal_driver)();
} peripheral_fetch;



const char* get_subsystem(subsystem_type system);

#endif