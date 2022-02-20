#include <zephyr.h>
#include <sys/printk.h>
#include <drivers/gpio.h>
#include "my_driver.h"


void main(void)
{	
	/* [NOTA] 'gpios' property (index 0) from 'green_led_2' node (which alias is 'led0') */
	const struct gpio_dt_spec led = GPIO_DT_SPEC_GET_BY_IDX(DT_ALIAS(led0), gpios, 0);
	
	const struct device *dev = device_get_binding("MY_DEVICE");
	
	printk("Test with board %s\n", CONFIG_BOARD);
	
	// Driver test
	printk("my_driver_do_this (%d)\n", my_driver_do_this(dev, 2, 3));
	my_driver_do_that(dev, NULL);
	
	// LED test
	gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	while (1)
	{
		gpio_pin_toggle_dt(&led);
		k_msleep(250);
	}
}

