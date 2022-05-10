#include "hal_fan.h"
#include "peripheral.h"


#define PERIOD 40

#define STEP 100    /* PWM pulse step */
#define MINPULSEWIDTH 20  /* Servo 0 degrees */
#define MIDDLEPULSEWIDTH 1500   /* Servo 90 degrees */
#define MAXPULSEWIDTH 2000  /* Servo 180 degrees */

static int num_fans;
static peripheral_device *fan_controls;
static peripheral_device *fan_feedbacks;



static void init() {
    
    struct device *control_fan_devices[] = { DT_FOREACH_CHILD_VARGS(DT_NODELABEL(fans), GET_DEVICE, control_device) };
    int control_fan_channels[] = { DT_FOREACH_CHILD_VARGS(DT_NODELABEL(fans), GET_CHANNEL, control_channel) };

    struct device *feeback_fan_devices[] = { DT_FOREACH_CHILD_VARGS(DT_NODELABEL(fans), GET_DEVICE, feedback_device) };
    int feeback_fan_channels[] = { DT_FOREACH_CHILD_VARGS(DT_NODELABEL(fans), GET_CHANNEL, feedback_pin) };

    num_fans = sizeof(control_fan_devices) / sizeof(int);
    fan_controls = k_calloc(num_fans, sizeof(peripheral_device));
    fan_feedbacks = k_calloc(num_fans, sizeof(peripheral_device));


    for (int i = 0; i < num_fans; i++) {
        fan_controls[i].access_type = CHANNEL;
        fan_controls[i].device = control_fan_devices[i];

        fan_controls[i].peripheral_access.channel = control_fan_channels[i];

        fan_feedbacks[i].access_type = CHANNEL;
        fan_feedbacks[i].device = feeback_fan_devices[i];
        fan_feedbacks[i].peripheral_access.channel = feeback_fan_channels[i];
    }
}


int hal_init_fan(int fan_number) {
    return 0;
    // return pwm_pin_enable_capture(fan_feedbacks[fan_number].device, fan_feedbacks[fan_number].peripheral_access.channel);;
}

int hal_init_fans(int *num_devices) {
    if (!fan_feedbacks)
        init();
    
    for (int i = 0; i < num_fans; i++) {
        if (hal_init_fan(i))
            return -1;
    }

    *num_devices = num_fans;
    
    hal_set_fan_speed(0, 100);

    return 0;
}

static int hal_get_fan_feedback(peripheral_device *fan) {
    uint64_t *period = k_calloc(1, sizeof(uint64_t));

    fan->data = k_calloc(1, sizeof(peripheral_datablock));
    fan->data->time = k_uptime_get();
    
    *period = k_uptime_get();
    /*
    memcpy(fan->data->status, "fail", strlen("fail"));

	if (pwm_pin_capture_usec(fan->device, fan->peripheral_access.channel, PWM_CAPTURE_TYPE_PERIOD, period, NULL, K_FOREVER)) {
        return -1;
    }
    */

    memcpy(fan->data->status, "okay", strlen("okay"));
    fan->data->information = (void *)period;

    return 0;
}

int hal_set_fan_speed(int fan_num, int duty_cycle) {
    printk("in here bruh %d %d \n", fan_num, duty_cycle);
    int pulse_width = (duty_cycle * PERIOD) / 100;
    return pwm_pin_set_usec(fan_controls[fan_num].device, fan_controls[fan_num].peripheral_access.channel, 
                    PERIOD, pulse_width, 0);
}

int hal_set_fans(int fan_num, int duty_cycle) {
    int ret = 0;

    if (fan_num != -1) 
        return hal_set_fan_speed(fan_num, duty_cycle);
    
    for (int i = 0; i < num_fans; i++) {
        ret += hal_set_fan_speed(i, duty_cycle);
    }
    
    return ret;
}

peripheral_datablock **hal_get_fan_feedbacks() {
    peripheral_datablock **records = k_calloc(num_fans, sizeof(peripheral_datablock *));

    for (int i = 0 ; i < num_fans; i++) {
        hal_get_fan_feedback(&fan_feedbacks[i]);
        
        if (!records)
            continue;
        
        records[i] = fan_feedbacks[i].data;
    }

    return records;
}