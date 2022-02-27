#include <drivers/sensor.h>
#include <logging/log.h>

#include "my_sensor.h"


#define DT_DRV_COMPAT st_my_sensor_press


LOG_MODULE_REGISTER(MY_SENSOR, CONFIG_SENSOR_LOG_LEVEL);


/* -------------------- my_sensor_sample_fetch -------------------- */
static int my_sensor_sample_fetch(const struct device *dev,
				enum sensor_channel chan)
{
	struct my_sensor_data *data = dev->data;
	const struct my_sensor_config *config = dev->config;
	uint8_t out[MY_SENSOR_PRESS_DATA_SIZE] = {0};
	
	__ASSERT_NO_MSG(chan == SENSOR_CHAN_PRESS);
	
	if (i2c_burst_read(data->i2c_master, config->i2c_slave_addr, MY_SENSOR_REG_PRESS_OUT, out, MY_SENSOR_PRESS_DATA_SIZE) < 0) {
		LOG_DBG("failed to read sample");
		return -EIO;
	}
	
	data->sample_press = (int32_t)(  ((uint32_t)(out[2]) << 16) | ((uint32_t)(out[1]) << 8) | out[0]  );
	
	return 0;
}


/* -------------------- my_sensor_channel_get -------------------- */
static int my_sensor_channel_get(const struct device *dev,
			       enum sensor_channel chan,
			       struct sensor_value *val)
{
	struct my_sensor_data *data = dev->data;
	
	__ASSERT_NO_MSG(chan == SENSOR_CHAN_PRESS);
	
	val->val1 = data->sample_press / 40960;
	val->val2 = ((int32_t)data->sample_press * 1000000 / 40960) % 1000000;
	
	return 0;
}


/* -------------------- sensor_driver_api -------------------- */
static const struct sensor_driver_api my_sensor_api_funcs = {
	.sample_fetch = my_sensor_sample_fetch,
	.channel_get = my_sensor_channel_get,
};


/* -------------------- my_sensor_poweroff -------------------- */
static void my_sensor_poweroff(const struct device *dev)
{
	struct my_sensor_data *data = dev->data;
	const struct my_sensor_config *config = dev->config;
	
	if (i2c_reg_write_byte(data->i2c_master, config->i2c_slave_addr, MY_SENSOR_REG_CTRL_REG1, 0x00) < 0) {
		LOG_DBG("failed to power off chip");
	}
}


/* -------------------- my_sensor_configure -------------------- */
static int my_sensor_configure(const struct device *dev)
{
	struct my_sensor_data *data = dev->data;
	const struct my_sensor_config *config = dev->config;
	uint8_t byte = 0;
	
	// Power up + Output data rate
	byte = (1 << 7) | ( (MY_SENSOR_DEFAULT_SAMPLING_RATE & 0x07) << 4 );
	if (i2c_reg_write_byte(data->i2c_master, config->i2c_slave_addr, MY_SENSOR_REG_CTRL_REG1, byte) < 0) {
		LOG_DBG("failed to set ODR");
		goto err_poweroff;
	}
#if 0
	// INT_DRDY active low
	byte = (1 << 7);
	if (i2c_reg_write_byte(data->i2c_master, config->i2c_slave_addr, MY_SENSOR_REG_CTRL_REG3, byte) < 0) {
		LOG_DBG("failed to set interrupt");
		goto err_poweroff;
	}
	
	// DRDY signal on INT_DRDY pin
	byte = 0x01;
	if (i2c_reg_write_byte(data->i2c_master, config->i2c_slave_addr, MY_SENSOR_REG_CTRL_REG4, byte) < 0) {
		LOG_DBG("failed to route interrupt");
		goto err_poweroff;
	}
#endif
	return 0;

err_poweroff:
	my_sensor_poweroff(dev);
	return -EIO;
}


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
		LOG_DBG("failed to read chip id");
		return -EIO;
	}
	
	if (chip_id != MY_SENSOR_VAL_WHO_AM_I) {
		LOG_DBG("invalid chip id 0x%x", chip_id);
		return -EIO;
	}
	
	// Configure chip
	if (my_sensor_configure(dev) < 0) {
		LOG_DBG("failed to configure chip");
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

