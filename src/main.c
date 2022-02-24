#include <zephyr.h>
#include <sys/printk.h>
#include <drivers/gpio.h>
#include "my_driver.h"


/* -------------------- FIFO -------------------- */
struct my_data_t {
	uint32_t cnt;
};

K_FIFO_DEFINE(my_fifo);


/* -------------------- my_tread -------------------- */
void my_tread(void)
{
	struct my_data_t my_data = {.cnt = 0};
	
	printk("Start my_thread\n");
	
	while (1)
	{
		k_fifo_put(&my_fifo, &my_data);
		my_data.cnt++;
		k_msleep(250);
	}
}

K_THREAD_DEFINE(my_tread_id, 1024, my_tread, NULL, NULL, NULL, 7, 0, 0);


/* -------------------- main [NOTA : more relevant in an application that would only requires a single thread] -------------------- */
void main(void)
{	
	/* [NOTA] 'gpios' property (index 0) from 'green_led_2' node (which alias is 'led0') */
	const struct gpio_dt_spec led = GPIO_DT_SPEC_GET_BY_IDX(DT_ALIAS(led0), gpios, 0);
	
	const struct device *dev = device_get_binding("MY_DEVICE");
	
	struct my_data_t *p_my_data = NULL;
	
	printk("Test with board %s\n", CONFIG_BOARD);
	
	// Driver test
	printk("my_driver_do_this (%d)\n", my_driver_do_this(dev, 2, 3));
	my_driver_do_that(dev, NULL);
	
	// LED test
	gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	while (1)
	{
		p_my_data = k_fifo_get(&my_fifo, K_FOREVER);
		gpio_pin_toggle_dt(&led);
		printk("data rcvd = %d\n", p_my_data->cnt);
	}
}

