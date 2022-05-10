#include "hal_door_sensor.h"
#include "peripheral.h"

static int num_doors;
static peripheral_device *door_sensors;




static void init() {
    
    struct device *door_sensor_devices[] = { DT_FOREACH_CHILD_VARGS(DT_NODELABEL(doors), GET_DEVICE, device) };
    int door_sensor_pins[] = { DT_FOREACH_CHILD_VARGS(DT_NODELABEL(doors), GET_CHANNEL, pin) };


    num_doors = sizeof(door_sensor_pins) / sizeof(int);
    door_sensors = k_calloc(num_doors, sizeof(peripheral_device));


    for (int i = 0; i < num_doors; i++) {
        door_sensors[i].access_type = PIN;
        door_sensors[i].device = door_sensor_devices[i];
        door_sensors[i].peripheral_access.pin = door_sensor_pins[i];
    }
}


int hal_init_door_sensor(int door_number) {
    
    return gpio_pin_configure(door_sensors[door_number].device, door_sensors[door_number].peripheral_access.pin, GPIO_PULL_UP | GPIO_INPUT);
}

int hal_init_door_sensors(int *num_devices) {
    if (!door_sensors)
        init();
    
    for (int i = 0; i < num_doors; i++) {
        if (hal_init_door_sensor(i))
            return -1;
    }

    *num_devices = door_sensors;

    return 0;
}

static int hal_get_door_sensor_status(peripheral_device *door) {
    return gpio_pin_get(door->device, door->peripheral_access.pin);
}



peripheral_datablock **hal_get_door_statuses() {
    peripheral_datablock **records = k_calloc(num_doors, sizeof(peripheral_datablock *));

    for (int i = 0 ; i < num_doors; i++) {
        hal_get_fan_feedback(&door_sensors[i]);
        
        if (!records)
            continue;
        
        records[i] = door_sensors[i].data;
    }

    return records;
}