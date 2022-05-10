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
#include <string.h>

#include "os_registry.h"
#include "registry.h"
#include "os_log.h"
#include <stdio.h>


#define UPDATE_STACKSIZE 512
#define GET_STACKSIZE 400
#define PRIORITY 2
#define UPDATE_PERIOD 200

K_MEM_SLAB_DEFINE(registry_update_queue_slab, sizeof(registry_update), 5, 0);
K_FIFO_DEFINE(registry_update_queue);

K_MEM_SLAB_DEFINE(registry_get_queue_slab, sizeof(registry_get), 1, 0);
K_FIFO_DEFINE(registry_get_queue);

K_MUTEX_DEFINE(fifo_lock);

static void TaskUpdateRegistry(void* p1, void* p2, void* p3);

K_THREAD_STACK_DEFINE(registry_update_stack_area, UPDATE_STACKSIZE);

static void TaskGetRegistry(void* p1, void* p2, void* p3);

K_THREAD_STACK_DEFINE(registry_get_stack_area, GET_STACKSIZE);

static k_tid_t registry_update_tid;
static struct k_thread registry_update_thread;

static k_tid_t registry_get_tid;
static struct k_thread registry_get_thread;

static registry *head =  NULL;
static registry *tail = NULL;

#define REGISTRY_DEPTH 10


/* 
 * Function used to start gps receiver thread. 
 * Returns 0 for success
 * 
 */
int os_init_registry() {

    registry_update_tid = k_thread_create(&registry_update_thread, registry_update_stack_area,\
                        K_THREAD_STACK_SIZEOF(registry_update_stack_area),\
                        TaskUpdateRegistry,\
                        NULL, NULL, NULL,\
                        PRIORITY, 0, K_NO_WAIT);

    registry_get_tid = k_thread_create(&registry_get_thread, registry_get_stack_area,\
                        K_THREAD_STACK_SIZEOF(registry_get_stack_area),\
                        TaskGetRegistry,\
                        NULL, NULL, NULL,\
                        PRIORITY, 0, K_NO_WAIT);
    return 0;
}

registry *register_peripheral(registry_update *update_info) {
    registry *registry = initialiase_registry(update_info->num_peripherals);
    printk("here? \n");
    registry->type = update_info->peripheral_type;

    if (tail == NULL) {
        printk("adding tail \n");
        tail = registry;
        return registry;
    }

    if (head == NULL) {
        printk("adding head \n");
        head = registry;
        tail->next_registry = head;
        return registry;
    }

    head->next_registry = registry;

    head = registry;

    return registry;
}

registry *find_registry(subsystem_type type) {
    
    for (registry *registry = tail; registry; registry = registry->next_registry) {
        if (registry->type == type)
            return registry;
    }

    return NULL;
}

static void log_subsystem_history(registry *registry, log_message *log_message) {
    int num_peripherals = registry->num_peripherals;

   

    // char **results[num_peripherals];
    char ***results = k_calloc(num_peripherals, sizeof(char **));
    int length_count = 0;
    printk("======================subsys ================\n");

    struct database *database;

    printk("got in here subsyste\n");

    for (int i = 0; i < num_peripherals; i++) {
        database = &registry->peripheral_databases[i];
        results[i] = log_database(registry, i);
        length_count += database->current_capacity;
    }

    char **combined_log = k_calloc(length_count, sizeof(char *));

    // printk("log count2 %d %p\n", length_count, combined_log);
    
    char **update_log = combined_log;

    for (int i = 0; i < num_peripherals; i++) {
        
        database = &registry->peripheral_databases[i];
        
        memcpy(update_log, results[i], sizeof(char *) * database->current_capacity);

        // printk("pointers %p %p \n", *update_log, results[i][0]);

        // for (int j = 0; j < database->current_capacity; j++) {
        //     printk("pointer %p \n", results[i][j]);
        //     k_free(results[i][j]);
        // }

        k_free(results[i]);

        update_log += database->current_capacity;
    }

    k_free(results);

    // printk("log count %d \n", length_count);

    log_message->num_entries = length_count;
    log_message->log_message = combined_log;

    // // for (int i = 0; i < length_count; i++) {
    // //     k_free(combined_log[i]);
    // // }

    // k_free(combined_log);
}

static void log_peripheral_history(registry *registry, int peripheral_id, log_message *log_message) {
    char **result = log_database(registry, peripheral_id);

    log_message->log_message = result;
    log_message->num_entries = registry->peripheral_databases[peripheral_id].current_capacity;
    //process
}

#include <stdbool.h>

static void log_single_value(registry *registry, int peripheral_id, log_message *log_message) {
    char **message_send = k_calloc(1, sizeof(char *));
    // char *result = log_entry(registry, peripheral_id);
    char *result = k_calloc(64, sizeof(char));

    printk("single value \n");

    sprintf(result, "=======tester mate============ \n");
    message_send[0] = result;
    log_message->log_message = message_send;
    log_message->num_entries = 1;
}

static void process_request(registry_get *get_info) {
    struct log_message log_message;
    printk("==========got in request ==============\n");
    registry *registry = find_registry(get_info->peripheral_type);
    int peripheral_id = get_info->peripheral_id;

    if (!registry)
        return;

    log_message.location = registry->type;

    switch (get_info->request_type)
    {
        case SINGLE_VALUE:
            log_single_value(registry, peripheral_id, &log_message);
            break;
        
        case PERIPHERAL_HISTORY:
            log_peripheral_history(registry, peripheral_id, &log_message);
            break;

        case SUBSYSTEM_HISTORY:
            log_subsystem_history(registry, &log_message);
            break;
    }

    queue_debug_message(&log_message);
}

static void process_update(registry_update *update_info) {
    registry *registry = find_registry(update_info->peripheral_type);

    // printk("got registry %p \n", registry);
    if (!registry)
        registry = register_peripheral(update_info);

    update_registry_info(registry, update_info->data);
}

/* 
 * GPS receiver task. Processes information from GPS receiver HAL driver
 * and if data is valid, places information on queue. 
 */
static void TaskUpdateRegistry(void *p1, void *p2, void *p3) {

    registry_update *update_info;
    registry_update update_info_copy;
    printk("starting update \n");

    for (;;) {

        k_mutex_lock(&fifo_lock, K_FOREVER);
       
        update_info = k_fifo_get(&registry_update_queue, K_FOREVER);
        printk("off queue \n");

        if (!update_info) {
            k_mutex_unlock(&fifo_lock);
            k_msleep(100);
            continue;
        }

        update_info_copy.peripheral_type = update_info->peripheral_type;
        update_info_copy.num_peripherals = update_info->num_peripherals;
        update_info_copy.data = update_info->data;
       

        k_mem_slab_free(&registry_update_queue_slab, (void **)&update_info);

        k_mutex_unlock(&fifo_lock);

        printk("updating now \n");

        process_update(&update_info_copy);

        if (update_info_copy.data != NULL) {
            printk("freeing\n");
            k_free(update_info_copy.data); 
        }
        k_msleep(100);
    }
}

static void TaskGetRegistry(void *p1, void *p2, void *p3) {
    registry_get *get_info;

    printk("starting get \n");

    for (;;) {

        get_info = k_fifo_get(&registry_get_queue, K_FOREVER);

        if (!get_info) {
            k_msleep(100);
            continue;
        }

        process_request(get_info);
        printk("out of get \n");

        k_mem_slab_free(&registry_get_queue_slab, (void **)&get_info);
        printk("past slab free");

        k_msleep(1000);
    }
}

void get_registry(registry_get *get_info) {
    registry_get *get_records;
    printk("in adding \n");

    if (k_mem_slab_alloc(&registry_get_queue_slab, (void **)&get_records, K_FOREVER) != 0) {
        printk("out of memory\n");
        return;
    }

    

    get_records->peripheral_id = get_info->peripheral_id;
    get_records->peripheral_type = get_info->peripheral_type;
    get_records->request_type = get_info->request_type;

    k_fifo_put(&registry_get_queue, (void *)get_records); 
}

void update_registry(registry_update *update_info) {
    registry_update *copy_info;
    registry_update *update_records;

    // k_mutex_lock(&fifo_lock, K_FOREVER);
    // if (!k_mem_slab_num_free_get(&registry_queue_slab)) {
        
    //     clear_fifo = k_fifo_get(&registry_queue, K_FOREVER);
        
    
    //     if (clear_fifo != NULL) {
    //         for (int i = 0; i < clear_fifo->num_peripherals; i++) {
    //             k_free(clear_fifo->registry_information.data[i]->information);
    //             k_free(clear_fifo->registry_information.data[i]);
    //         }
            
    //         k_free(clear_fifo->registry_information.data);
    //         k_mem_slab_free(&registry_queue_slab, (void **)&clear_fifo);
    //     }
    // }
    // k_mutex_unlock(&fifo_lock);

    printk("in here \n");

    if (k_mem_slab_alloc(&registry_update_queue_slab, (void **)&update_records, K_NO_WAIT) != 0) {
        printk("out of memory\n");
        return;
    }

    update_records->peripheral_type = update_info->peripheral_type;
    update_records->num_peripherals = update_info->num_peripherals;

    update_records->data = update_info->data;


    // printk("in queue : %lld \n", registry_comms->registry_information.data[0]->time);
    printk("in here \n");
    k_fifo_put(&registry_update_queue, (void *)update_records); 

}



