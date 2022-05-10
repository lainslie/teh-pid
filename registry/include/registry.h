#ifndef _REGISTRY_H
#define _REGISTRY_H

#include "peripheral.h"

struct database_entry {
    struct database_entry *previous_entry;
    struct database_entry *next_entry;

    peripheral_datablock *data;
}typedef database_entry;

typedef struct database {
    int max_capacity;
    int current_capacity;
    database_entry *head;
    database_entry *tail;
    database_entry *current_position;
    peripheral_datablock **sorted_data;
} database;

typedef struct registry {
    int num_peripherals;
    database *peripheral_databases;

    subsystem_type type;
    int registry_length;

    struct registry *next_registry;
} registry;

registry * initialiase_registry(int num_peripherals);
void update_registry_info(registry *registry, peripheral_datablock **data);
void print_registry(registry *registry);
peripheral_datablock **get_registry_entries(registry *registry);
peripheral_datablock *get_current_entry(registry *registry, int peripheral_number);

char **log_database(registry *registry, int peripheral_id);
char *log_entry(peripheral_datablock *data_entry, int peripheral_id);

#endif