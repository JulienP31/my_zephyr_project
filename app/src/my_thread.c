#include <zephyr.h>
#include <sys/printk.h>
#include <drivers/gpio.h>


void my_tread(void)
{
	/* [NOTA] 'gpios' property (index 0) from 'green_led_2' node (which alias is 'led0') */
	const struct gpio_dt_spec led = GPIO_DT_SPEC_GET_BY_IDX(DT_ALIAS(led0), gpios, 0);
	
	printk("Starting my_thread\n");
	
	// LED test
	if ( !device_is_ready(led.port) ) {
		printk("Could not get LED\n");
		return;
	}
	
	gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	
	while (1) {
		gpio_pin_toggle_dt(&led);
		
		k_msleep(250);
	}
}

K_THREAD_DEFINE(my_tread_id, 1024, my_tread, NULL, NULL, NULL, 7, 0, 0);

