#include "hal_light_sensor.h"
#include "peripheral.h"


#if !DT_NODE_HAS_STATUS(DT_NODELABEL(light_sensors), okay) 
#error "Sensors not correctly configured in device tree"
#endif

static peripheral_device *light_sensors = NULL;
static int num_peripherals;

static int16_t m_sample_buffer[BUFFER_SIZE];

// the channel configuration with channel not yet filled in
static struct adc_channel_cfg channel_cfg = {
	.gain             = ADC_GAIN,
	.reference        = ADC_REFERENCE,
	.acquisition_time = ADC_ACQUISITION_TIME,
	.channel_id       = 0, // gets set during init
};

static struct adc_sequence sequence = {
		.options     = NULL,				// extra samples and callback
		.channels    = BIT(0),		// bit mask of channels to read
		.buffer      = m_sample_buffer,		// where to put samples read
		.buffer_size = sizeof(m_sample_buffer),
		.resolution  = ADC_RESOLUTION,		// desired resolution
		.oversampling = 0,					// don't oversample
		.calibrate = 0						// don't calibrate
};

static int hal_get_light_sensor_reading(peripheral_device *sensor) {
    int32_t adc_vref;
    int32_t raw_reading;
    printk("mallocing\n");

    struct device *adc_device = sensor->device;
    int channel_num = sensor->peripheral_access.channel;

    sensor->data = k_malloc(sizeof(peripheral_datablock));
    printk("mallocing\n");
    sensor->data->time = k_uptime_get();

    memcpy(sensor->data->status, "fail", strlen("fail")); 

    printk("pointerrrrr \n");

    if (!(adc_vref = adc_ref_internal(adc_device))) {
        //log err
        return -1;
    }

    sequence.channels = BIT(channel_num);

    if (adc_read(adc_device, &sequence)) {
        //log err
        return -1;
    }

    // appears to be a zephyr bug
    raw_reading = m_sample_buffer[0];

    if (adc_raw_to_millivolts(adc_vref, ADC_GAIN, ADC_RESOLUTION, &raw_reading)) {
        //log err
        return -1;
    }

    memset(sensor->data->status, 0, 5);
    memcpy(sensor->data->status, "okay", strlen("okay"));

    int data_length = snprintf(NULL, 0, "%d Volts", raw_reading);

    sensor->data->information = k_calloc(data_length, sizeof(char));

    sprintf(sensor->data->information, "%d Volts", raw_reading);

    return 0;
}

int hal_init_sensor(int sensor_number) {
    channel_cfg.channel_id = light_sensors[sensor_number].peripheral_access.channel;

    if (adc_channel_setup(light_sensors[sensor_number].device, &channel_cfg)) {
        printk("sensor init failed \n");
        //log err
        return -1;
    }

    return 0;
}

static void init() {
    struct device *light_sensor_devices[] = { DT_FOREACH_CHILD_VARGS(DT_NODELABEL(light_sensors), GET_DEVICE, device) };
    int light_sensor_channels[] = { DT_FOREACH_CHILD_VARGS(DT_NODELABEL(light_sensors), GET_CHANNEL, channel) };

    num_peripherals = sizeof(light_sensor_channels) / sizeof(int);
    light_sensors = k_malloc(sizeof(peripheral_device) * num_peripherals);

    for (int i = 0; i < num_peripherals; i++) {
        light_sensors[i].access_type = CHANNEL;
        light_sensors[i].device = light_sensor_devices[i];
        light_sensors[i].peripheral_access.channel = light_sensor_channels[i];
    }
}

int hal_init_light_sensors(int *num_devices) {
    if (!light_sensors)
        init();
    
    for (int i = 0; i < num_peripherals; i++) {
        if (hal_init_sensor(i))
            return -1;
    }

    *num_devices = num_peripherals;
    return 0;
}

peripheral_datablock **hal_get_light_sensors_reading() {
    peripheral_datablock **records = k_malloc(sizeof(peripheral_datablock *) * num_peripherals);
    printk("mallocing\n");
    
    for (int i = 0 ; i < num_peripherals; i++) {
        hal_get_light_sensor_reading(&light_sensors[i]);
        
        if (!records)
            continue;
        
        records[i] = light_sensors[i].data;
    }

    return records;
}


