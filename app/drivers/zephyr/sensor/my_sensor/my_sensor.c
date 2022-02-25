#include <drivers/sensor.h>
#include <logging/log.h>

#include "my_sensor.h"


#define DT_DRV_COMPAT st_my_sensor_press


LOG_MODULE_REGISTER(MY_SENSOR, CONFIG_SENSOR_LOG_LEVEL);


/* -------------------- my_sensor_sample_fetch -------------------- */
static int my_sensor_sample_fetch(const struct device *dev,
				enum sensor_channel chan)
{
	

	return 0;
}


/* -------------------- my_sensor_channel_get -------------------- */
static int my_sensor_channel_get(const struct device *dev,
			       enum sensor_channel chan,
			       struct sensor_value *val)
{
	val->val1 = 123;
	val->val2 = 456789;

	return 0;
}


/* -------------------- sensor_driver_api -------------------- */
static const struct sensor_driver_api my_sensor_api_funcs = {
	.sample_fetch = my_sensor_sample_fetch,
	.channel_get = my_sensor_channel_get,
};


/* -------------------- my_sensor_init -------------------- */
static int my_sensor_init(const struct device *dev)
{
	

	return 0;
}


/* -------------------- struct -------------------- */
static const struct my_sensor_config my_sensor_config = {
	.dummy = 0
};


static struct my_sensor_data my_sensor_data = {
	.dummy = 0
};


DEVICE_DT_INST_DEFINE(0, my_sensor_init, NULL,
		    &my_sensor_data, &my_sensor_config, POST_KERNEL,
		    CONFIG_SENSOR_INIT_PRIORITY, &my_sensor_api_funcs);

