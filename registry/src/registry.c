#include "registry.h"
#include <zephyr.h>
#include <string.h>
#include <stdio.h>

#define REGISTRY_DEPTH 5

void initialise_database(database *database);
void add_database_entry(database *database, peripheral_datablock *data);
void print_database(database *database);


registry * initialiase_registry(int num_peripherals) {
    registry *registry = (struct registry *)k_calloc(1, sizeof(struct registry));
    registry->next_registry = NULL;
    
    registry->num_peripherals = num_peripherals;
    registry->peripheral_databases = (struct database *)k_calloc(num_peripherals, sizeof(struct database));

    for (int i = 0; i < num_peripherals; i++) {
        
        registry->peripheral_databases[i].max_capacity = REGISTRY_DEPTH;
        initialise_database(&registry->peripheral_databases[i]);
    }

    return registry;
}

void initialise_database(database *database) {
    database->tail = k_calloc(1, sizeof(database_entry));
    database->tail->data = NULL;
    
    database_entry *reg_ptr;
    database_entry *previous_ptr = database->tail;

    if (!database->max_capacity)
        return;

    database->sorted_data = (peripheral_datablock **)k_calloc(database->max_capacity, sizeof(peripheral_datablock *));

    for (int i = 0; i < (database->max_capacity - 1); i++) {
        reg_ptr = k_calloc(1, sizeof(database_entry));
        reg_ptr->data = NULL;
        reg_ptr->previous_entry = previous_ptr;
        previous_ptr->next_entry = reg_ptr;

        previous_ptr = reg_ptr;
    }
    database->head = reg_ptr;
    database->head->next_entry = database->tail;
    database->current_position = database->tail;
    
}

void update_registry_info(registry *registry, peripheral_datablock **data) {
    for (int i = 0; i < registry->num_peripherals; i++) {
        printk("==========%d======== \n", i);
        add_database_entry(&registry->peripheral_databases[i], data[i]);
    }
}

void sort_database(database *database);

void add_database_entry(database *database, peripheral_datablock *data) {
    database_entry *current = database->current_position;

    if (!data) {
        return;
    }

    if (database->current_capacity != database->max_capacity)
        database->current_capacity++;

    if (!database->tail->data) {
        database->tail->data = data;
        return;
    }

    if (current->next_entry->data) {
        printk("freeing\n");
        k_free(current->next_entry->data->information);
        printk("freeing\n");
        k_free(current->next_entry->data);
    }

    // printk("success \n");

    current->next_entry->data = data;

    database->current_position = current->next_entry;

    

    sort_database(database);
    // print_database(database);
}

void sort_database(database *database) {
    peripheral_datablock **sorted_data = database->sorted_data;
    database_entry *next_ptr;
    database_entry *stop_ptr = database->current_position;

    for (next_ptr = stop_ptr->next_entry ; (next_ptr != stop_ptr); next_ptr = next_ptr->next_entry) {
        if (!next_ptr->data)
            continue;
        *sorted_data++ = next_ptr->data;
    }

    if (stop_ptr->data)
        *sorted_data = stop_ptr->data;
}


void print_database(database *database) {
    int *reading;
    for (int i = 0; i < database->current_capacity; i++) {
    
        if(!database->sorted_data[i])
            break;
        reading = (int *)database->sorted_data[i]->information;
        printk("reading: %d \n", *reading);
    }
}

char *log_entry(peripheral_datablock *data_entry, int peripheral_id) {
    char *log_entry;
    // printk("ver critical %p \n", data_entry->information);

    int prefix_length = snprintf(NULL, 0, "%d,%lld,%s", peripheral_id, data_entry->time, data_entry->status);

    char *prefix = k_calloc(prefix_length + 1, sizeof(char));

    snprintf(prefix, prefix_length + 1, "%d,%lld,%s", peripheral_id, data_entry->time, data_entry->status);

    // printk("important: %s %d \n", data_entry->information, strlen(data_entry->information));

    log_entry = k_calloc(strlen(prefix) + strlen(data_entry->information) + 2, sizeof(char)); 

    // printk("ver critical %p %p\n", log_entry, prefix);

    snprintf(log_entry, strlen(prefix) + strlen(data_entry->information) + 2, "%s,%s\n", prefix, data_entry->information);

    k_free(prefix);   

    return log_entry;
}

char **log_database(registry *registry, int peripheral_id) { 
    database *data = &registry->peripheral_databases[peripheral_id];

    char **database_log = k_calloc(data->current_capacity, sizeof(char *));
    
    for (int i = 0; i < data->current_capacity; i++) {
        if(!data->sorted_data[i])
            break;

        peripheral_datablock *sorted_data = data->sorted_data[i];

        database_log[i] = log_entry(sorted_data, peripheral_id);
    }

    return database_log;
}


peripheral_datablock **get_database(registry *registry, int peripheral_number) {
    struct database *database = &registry->peripheral_databases[peripheral_number];
    sort_database(database);

    return database->sorted_data;
}

peripheral_datablock *get_current_entry(registry *registry, int peripheral_number) {
    return registry->peripheral_databases[peripheral_number].current_position->data;
}

void deinitialise_database(database *database) {
    database_entry *next_ptr;
    database_entry *tail = database->tail;

    for (database_entry *current_ptr = database->tail->next_entry; current_ptr != tail; current_ptr = next_ptr) {
        next_ptr = current_ptr->next_entry;
        k_free(current_ptr->data);
        k_free(current_ptr);
    }

    k_free(tail);
    k_free(database->sorted_data);
}