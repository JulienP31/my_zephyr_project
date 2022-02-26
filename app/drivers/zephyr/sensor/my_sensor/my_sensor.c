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
	const struct my_sensor_config * const config = dev->config;
	struct my_sensor_data *data = dev->data;
	uint8_t chip_id = 0;
	
	// Get I2C master device
	data->i2c_master = device_get_binding(config->i2c_master_dev_name);
	if (!data->i2c_master) {
		LOG_DBG("i2c master not found: %s", config->i2c_master_dev_name);
		return -EINVAL;
	}
	
	// Check chip ID
	if (i2c_reg_read_byte(data->i2c_master, config->i2c_slave_addr, MY_SENSOR_REG_WHO_AM_I, &chip_id) < 0) {
		LOG_DBG("failed reading chip id");
		return -EIO;
	}
	
	if (chip_id != MY_SENSOR_VAL_WHO_AM_I) {
		LOG_DBG("invalid chip id 0x%x", chip_id);
		return -EIO;
	}
	
	return 0;
}


/* -------------------- struct -------------------- */
static const struct my_sensor_config my_sensor_config = {
	.i2c_master_dev_name = DT_INST_BUS_LABEL(0),
	.i2c_slave_addr = DT_INST_REG_ADDR(0),
};


static struct my_sensor_data my_sensor_data;


DEVICE_DT_INST_DEFINE(0, my_sensor_init, NULL,
		    &my_sensor_data, &my_sensor_config, POST_KERNEL,
		    CONFIG_SENSOR_INIT_PRIORITY, &my_sensor_api_funcs);

