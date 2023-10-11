#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>


#define POTENTIOMETER_NODE	potentiometer
#define ADC_NODE		DT_PHANDLE(DT_PATH(POTENTIOMETER_NODE), io_channels)


void my_tread(void)
{
	/* [NOTA] 'gpios' property (index 0) from 'green_led_2' node (which alias is 'led0') */
	const struct gpio_dt_spec led = GPIO_DT_SPEC_GET_BY_IDX(DT_ALIAS(led0), gpios, 0);
	
	int16_t sample_val = 0;
	
	struct adc_channel_cfg channel_cfg = {
		.gain             = ADC_GAIN_1,
		.reference        = ADC_REF_INTERNAL,
		.acquisition_time = ADC_ACQ_TIME_DEFAULT,
		.channel_id       = DT_IO_CHANNELS_INPUT_BY_IDX(DT_PATH(POTENTIOMETER_NODE), 0),
		.differential     = 0
	};

	struct adc_sequence sequence = {
		.channels    = BIT(channel_cfg.channel_id),
		.buffer      = &sample_val,
		.buffer_size = sizeof(sample_val), //< buffer size in bytes, not number of samples
		.resolution  = 12,
	};
	
	const struct device *adc = DEVICE_DT_GET(ADC_NODE);
	
	int err = 0;
	
	printk("Starting my_thread\n");
	
	// LED test
	if ( !device_is_ready(led.port) ) {
		printk("Could not get LED\n");
		return;
	}
	
	gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	
	// ADC test
	if ( !device_is_ready(adc) ) {
		printk("Could not get ADC\n");
		return;
	}
	
	adc_channel_setup(adc, &channel_cfg);
	
	while (1) {
		err = adc_read(adc, &sequence);
		if (err != 0) {
			printk("ADC reading failed with error %d\n", err);
			return;
		}
		
		printk("ADC sample = %d\n", sample_val);
		
		gpio_pin_toggle_dt(&led);
		
		k_msleep(250);
	}
}

K_THREAD_DEFINE(my_tread_id, 1024, my_tread, NULL, NULL, NULL, 7, 0, 0);

