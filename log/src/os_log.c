#include "os_log.h"
#include <shell/shell.h>
#include <stdio.h>

#define STACKSIZE 256
#define PRIORITY 7

K_FIFO_DEFINE(debug_log_queue);

K_MEM_SLAB_DEFINE(debug_log_slab, sizeof(log_message), 10, 0);

K_FIFO_DEFINE(registry_log_queue);

K_MEM_SLAB_DEFINE(registry_log_slab, sizeof(log_message), 1, 0);

#define DEBUG_LOG 0

K_THREAD_STACK_DEFINE(debug_log_stack_area, STACKSIZE);
static k_tid_t debug_log_tid;
static struct k_thread debug_log_thread;

K_THREAD_STACK_DEFINE(registry_log_stack_area, STACKSIZE);
static k_tid_t registry_log_tid;
static struct k_thread registry_log_thread;
void TaskComLog(void *p1, void *p2, void *p3);
void TaskDebugLog(void *p1, void *p2, void *p3);


/* 
 * Function used to start gps receiver thread. 
 * Returns 0 for success
 * 
 */
int os_init_logger() {

    debug_log_tid = k_thread_create(&debug_log_thread, debug_log_stack_area,\
                        K_THREAD_STACK_SIZEOF(debug_log_stack_area),\
                        TaskDebugLog,\
                        NULL, NULL, NULL,\
                        PRIORITY, 0, K_NO_WAIT);

    registry_log_tid = k_thread_create(&registry_log_thread, registry_log_stack_area,\
                        K_THREAD_STACK_SIZEOF(registry_log_stack_area),\
                        TaskComLog,\
                        NULL, NULL, NULL,\
                        PRIORITY, 0, K_NO_WAIT);
    return 0;
}

void TaskComLog(void *p1, void *p2, void *p3) {
    log_message *message;

    for (;;) {

        if (k_fifo_is_empty(&registry_log_queue)) {
            k_msleep(100);
            continue;
        }

        message = k_fifo_get(&registry_log_queue, K_NO_WAIT);

        if (!message) {
            k_msleep(100);
            continue;
        }

        //todo use dt for shell_backend_uart_get_ptr
        for (int i = 0; i < message->num_entries; i++) {
            shell_print(shell_backend_uart_get_ptr(), "%s", message->log_message[i]);
            k_free(message->log_message[i]);
        }

        k_free(message->log_message);
         
        k_mem_slab_free(&registry_log_slab, (void **)&message);

        k_msleep(100);
    }

}


void TaskDebugLog(void *p1, void *p2, void *p3) {

    log_message *debug_message;

    for (;;) {
        if (k_fifo_is_empty(&debug_log_queue)) {
            k_msleep(100);
            continue;
        }

        debug_message = k_fifo_get(&debug_log_queue, K_NO_WAIT);

        if (!debug_message) {
            printk("didnt work \n");
            k_msleep(100);
            continue;
        }

        for (int i = 0; i < debug_message->num_entries; i++) {
            printk("%s\n", debug_message->log_message[i]);
            k_free(debug_message->log_message[i]);
        }

        k_free(debug_message->log_message);
         
        k_mem_slab_free(&debug_log_slab, (void **)&debug_message);

        k_msleep(100);
    }
}

void log_print(log_type message_type, subsystem_type location, int peripheral_id, char *message) {
    log_message debug_message;
    char *log_message;

    // printk("message %s \n", message);

    int message_length = snprintf(NULL, 0, "%lld %s %d %s\n", k_uptime_get(), get_subsystem(location), peripheral_id, message);

    log_message = (char *)k_calloc(message_length + 1, sizeof(char));

    sprintf(log_message, "%lld %s %d %s\n", k_uptime_get(), get_subsystem(location), peripheral_id, message);

    debug_message.num_entries = 1;
    debug_message.log_message = k_calloc(1, sizeof(char *));
    debug_message.log_message[0] = log_message;

    debug_message.location = location;

    printk("log: %s \n", log_message);

    //fill out log message

    queue_debug_message(&debug_message);
}



void queue_debug_message(log_message *message) {

    log_message *debug_message;
    


    if (k_mem_slab_alloc(&debug_log_slab, (void **)&debug_message, K_FOREVER) != 0) {
        printk("out of memory\n");
        return;
    }

    debug_message->num_entries = message->num_entries;
    debug_message->log_message = message->log_message;
    debug_message->location = message->location;



    k_fifo_put(&debug_log_queue, (void *)debug_message); 
}

void queue_registry_message(log_message *message) {

    log_message *registry_message;
    


    if (k_mem_slab_alloc(&registry_log_slab, (void **)&registry_message, K_FOREVER) != 0) {
        printk("out of memory\n");
        return;
    }

    registry_message->num_entries = message->num_entries;
    registry_message->log_message = message->log_message;
    registry_message->location = message->location;



    k_fifo_put(&registry_log_queue, (void *)registry_message);
}