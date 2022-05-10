#ifndef _OS_REGISTRY_H
#define _OS_REGISTRY_H

#include "peripheral.h"
#include "os_log.h"
#include <zephyr.h>


typedef struct registry_update {
    void *reserved;

    subsystem_type peripheral_type;

    int num_peripherals;
    
    peripheral_datablock **data;
} registry_update;

typedef struct registry_get {
    void *reserved;

    int peripheral_id;

    subsystem_type peripheral_type;

    log_query request_type;
} registry_get;

// typedef struct registry_comms {
//     void *reserved;
//     enum subsystem_type peripheral_type;

//     enum registry_comms_type comms_type;

//     enum log_query request_type;

//     int num_peripherals;
    
//     union {
//         int peripheral_id;
//         peripheral_datablock **data;
//     } registry_information;

// } registry_comms;

int os_init_registry();


void update_registry(registry_update *update_info);
void get_registry(registry_get *get_info);

#endif