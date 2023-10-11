#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/sensor.h>
#include "my_driver.h"


/* -------------------- FIFO -------------------- */
typedef struct my_fifo_data {
	struct sensor_value press;
} my_data_t;

static my_data_t my_data;

K_FIFO_DEFINE(my_fifo);


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


/* -------------------- main [NOTA : more relevant in an application that would only requires a single thread] -------------------- */
int main(void)
{	
	const struct device *dev = device_get_binding("MY_DEVICE");
	
	const struct device *lps25hb = DEVICE_DT_GET(DT_NODELABEL(my_sensor));
	struct sensor_trigger trig = {0};
	
	my_data_t *p_my_data = NULL;
	
	printk("Test with board %s\n", CONFIG_BOARD);
	
	// Driver test
	if ( !device_is_ready(dev) ) {
		printk("Could not get MY_DEVICE\n");
		return -1;
	}
	
	printk("my_driver_do_this (%d)\n", my_driver_do_this(dev, 2, 3));
	my_driver_do_that(dev, NULL);
	
	// Sensor test (consumer)
	trig.type = SENSOR_TRIG_DATA_READY;
	trig.chan = SENSOR_CHAN_PRESS;
	sensor_trigger_set(lps25hb, &trig, my_sensor_trigger_handler); /* [NOTA] Handler will be called from a thread, so I2C or SPI operations are safe */
	
	while (1) {
		p_my_data = k_fifo_get(&my_fifo, K_FOREVER);
		printk("pressure = %d | %d\n", p_my_data->press.val1, p_my_data->press.val2);
	}
	
	return -1;
}

