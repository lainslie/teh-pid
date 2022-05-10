#ifndef _OS_LOG_H
#define _OS_LOG_H

#include <zephyr.h>
#include <devicetree.h>
#include "peripheral.h"
#include "log.h"

typedef enum log_type {
    LOG_DEBUG, 
    LOG_INFO, 
    LOG_ERROR, 
    LOG_WARN
} log_type;

typedef struct log_message {
    uint64_t time;
    // enum log_type message_type;

    subsystem_type location;
    int peripheral_id;


    char **log_message;
    int num_entries;

} log_message; 

int os_init_logger();

void log_print(log_type message_type, subsystem_type location, int peripheral_id, char *message);

void queue_debug_message(log_message *message);
void queue_registry_message(log_message *message);

#endif