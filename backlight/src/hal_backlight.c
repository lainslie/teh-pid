#include "hal_backlight.h"


#include "peripheral.h"

static int num_backlights;
static peripheral_device *backlights;




static void init() {
    
    struct device *backlight_devices[] = { DT_FOREACH_CHILD_VARGS(DT_NODELABEL(backlights), GET_DEVICE, device) };
    int backlight_pins[] = { DT_FOREACH_CHILD_VARGS(DT_NODELABEL(backlights), GET_CHANNEL, pin) };


    num_backlights = sizeof(backlight_pins) / sizeof(int);
    backlights = k_calloc(num_backlights, sizeof(peripheral_device));


    for (int i = 0; i < num_backlights; i++) {
        backlights[i].access_type = PIN;
        backlights[i].device = backlight_devices[i];
        backlights[i].peripheral_access.pin = backlight_pins[i];
    }
}


int hal_init_backlight(int backlight_number) {
    
    return gpio_pin_configure(backlights[backlight_number].device, backlights[backlight_number].peripheral_access.pin, GPIO_PULL_UP | GPIO_INPUT);
}

int hal_init_backlights(int *num_devices) {
    if (!backlights)
        init();
    
    for (int i = 0; i < num_backlights; i++) {
        if (hal_init_backlight(i))
            return -1;
    }

    *num_devices = num_backlights;

    return 0;
}

static int hal_get_backlight_status(peripheral_device *backlight) {
    return gpio_pin_get(backlight->device, backlight->peripheral_access.pin);
}



peripheral_datablock **hal_get_backlight_statuses() {
    peripheral_datablock **records = k_calloc(num_backlights, sizeof(peripheral_datablock *));

    for (int i = 0 ; i < num_backlights; i++) {
        hal_get_backlight_status(&backlights[i]);
        
        if (!records)
            continue;
        
        records[i] = backlights[i].data;
    }

    return records;
}