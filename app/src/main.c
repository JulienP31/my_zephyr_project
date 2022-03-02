#include <zephyr.h>
#include <sys/printk.h>
#include <drivers/gpio.h>
#include <drivers/sensor.h>
#include "my_driver.h"


/* -------------------- FIFO -------------------- */
typedef struct my_fifo_data {
	struct sensor_value press;
} my_data_t;

static my_data_t my_data;

K_FIFO_DEFINE(my_fifo);


#if 0
/* -------------------- my_tread [NOTA : obsolete] -------------------- */
void my_tread(void)
{
	const struct device *lps25hb = device_get_binding(DT_LABEL(DT_INST(0, st_my_sensor_press)));
	
	printk("Starting my_thread\n");
	
	// Sensor test (producer)
	if ( !device_is_ready(lps25hb) ) {
		printk("Could not get LPS25HB device\n");
		return;
	}
	
	while (1) {
		if ( sensor_sample_fetch(lps25hb) < 0 ) {
			printk("LPS25HB sensor sample fetch error\n");
			return;
		}
		
		sensor_channel_get(lps25hb, SENSOR_CHAN_PRESS, &(my_data.press));
		
		k_fifo_put(&my_fifo, &my_data);
		
		k_msleep(2000);
	}
}

K_THREAD_DEFINE(my_tread_id, 1024, my_tread, NULL, NULL, NULL, 7, 0, 0);

#else
/* -------------------- my_sensor_trigger_handler -------------------- */
static void my_sensor_trigger_handler(const struct device *dev, const struct sensor_trigger *trig)
{
	// Sensor test (producer)
	if ( sensor_sample_fetch(dev) < 0 ) {
		printk("LPS25HB sensor sample fetch error\n");
		return;
	}
	
	sensor_channel_get(dev, SENSOR_CHAN_PRESS, &(my_data.press));
	
	k_fifo_put(&my_fifo, &my_data);
}

#endif


/* -------------------- main [NOTA : more relevant in an application that would only requires a single thread] -------------------- */
void main(void)
{
	/* [NOTA] 'gpios' property (index 0) from 'green_led_2' node (which alias is 'led0') */
	const struct gpio_dt_spec led = GPIO_DT_SPEC_GET_BY_IDX(DT_ALIAS(led0), gpios, 0);
	
	const struct device *dev = device_get_binding("MY_DEVICE");
	
	const struct device *lps25hb = device_get_binding(DT_LABEL(DT_INST(0, st_my_sensor_press)));
	struct sensor_trigger trig = {0};
	
	my_data_t *p_my_data = NULL;
	
	printk("Test with board %s\n", CONFIG_BOARD);
	
	// Driver test
	if ( !device_is_ready(dev) ) {
		printk("Could not get MY_DEVICE\n");
		return;
	}
	
	printk("my_driver_do_this (%d)\n", my_driver_do_this(dev, 2, 3));
	my_driver_do_that(dev, NULL);
	
	// LED test
	if ( !device_is_ready(led.port) ) {
		printk("Could not get LED\n");
		return;
	}
	
	gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	
	// Sensor test (consumer)
	trig.type = SENSOR_TRIG_DATA_READY;
	trig.chan = SENSOR_CHAN_PRESS;
	sensor_trigger_set(lps25hb, &trig, my_sensor_trigger_handler); /* [NOTA] Handler will be called from a thread, so I2C or SPI operations are safe */
	
	while (1) {
		p_my_data = k_fifo_get(&my_fifo, K_FOREVER);
		printk("pressure = %d | %d\n", p_my_data->press.val1, p_my_data->press.val2);
		
		gpio_pin_toggle_dt(&led);
	}
}

