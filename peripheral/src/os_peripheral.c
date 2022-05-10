#include "os_peripheral.h"
#include "peripheral.h"
#include "os_registry.h"

#define RETRIEVE_RATE 100


void peripheral_tracking(void *p1, void *p2, void *p3) {

    peripheral_fetch *tracker = (peripheral_fetch *)p1;
    registry_update update_info = {.num_peripherals = tracker->num_peripherals, .peripheral_type = tracker->peripheral_type};
    
    printk("info %d \n", tracker->num_peripherals);
    for (;;) {
        printk("started peripheral \n");
        update_info.data = tracker->hal_driver();
        printk("got readings \n");
        printk("about to update registry \n");
        
        update_registry(&update_info);

        k_msleep(RETRIEVE_RATE);
    }
}