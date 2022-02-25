/* my_sensor.c - Driver for MY_SENSOR pressure and temperature sensor */

#define DT_DRV_COMPAT st_my_sensor_press

#include <drivers/sensor.h>
#include <kernel.h>
#include <device.h>
#include <init.h>
#include <sys/byteorder.h>
#include <sys/__assert.h>
#include <logging/log.h>

#include "my_sensor.h"

LOG_MODULE_REGISTER(MY_SENSOR, CONFIG_SENSOR_LOG_LEVEL);

static inline int my_sensor_power_ctrl(const struct device *dev, uint8_t value)
{
	struct my_sensor_data *data = dev->data;
	const struct my_sensor_config *config = dev->config;

	return i2c_reg_update_byte(data->i2c_master, config->i2c_slave_addr,
				   MY_SENSOR_REG_CTRL_REG1,
				   MY_SENSOR_MASK_CTRL_REG1_PD,
				   value << MY_SENSOR_SHIFT_CTRL_REG1_PD);
}

static inline int my_sensor_set_odr_raw(const struct device *dev, uint8_t odr)
{
	struct my_sensor_data *data = dev->data;
	const struct my_sensor_config *config = dev->config;

	return i2c_reg_update_byte(data->i2c_master, config->i2c_slave_addr,
				   MY_SENSOR_REG_CTRL_REG1,
				   MY_SENSOR_MASK_CTRL_REG1_ODR,
				   odr << MY_SENSOR_SHIFT_CTRL_REG1_ODR);
}

static int my_sensor_sample_fetch(const struct device *dev,
				enum sensor_channel chan)
{
	struct my_sensor_data *data = dev->data;
	const struct my_sensor_config *config = dev->config;
	uint8_t out[5];
	int offset;

	__ASSERT_NO_MSG(chan == SENSOR_CHAN_ALL);

	for (offset = 0; offset < sizeof(out); ++offset) {
		if (i2c_reg_read_byte(data->i2c_master, config->i2c_slave_addr,
				      MY_SENSOR_REG_PRESS_OUT_XL + offset,
				      out + offset) < 0) {
			LOG_DBG("failed to read sample");
			return -EIO;
		}
	}

	data->sample_press = (int32_t)((uint32_t)(out[0]) |
					((uint32_t)(out[1]) << 8) |
					((uint32_t)(out[2]) << 16));
	data->sample_temp = (int16_t)((uint16_t)(out[3]) |
					((uint16_t)(out[4]) << 8));

	return 0;
}

static inline void my_sensor_press_convert(struct sensor_value *val,
					 int32_t raw_val)
{
	/* val = raw_val / 40960 */
	val->val1 = raw_val / 40960;
	val->val2 = ((int32_t)raw_val * 1000000 / 40960) % 1000000;
}

static inline void my_sensor_temp_convert(struct sensor_value *val,
					int16_t raw_val)
{
	int32_t uval;

	/* val = raw_val / 480 + 42.5 */
	uval = (int32_t)raw_val * 1000000 / 480 + 42500000;
	val->val1 = (raw_val * 10 / 480 + 425) / 10;
	val->val2 = uval % 1000000;
}

static int my_sensor_channel_get(const struct device *dev,
			       enum sensor_channel chan,
			       struct sensor_value *val)
{
	struct my_sensor_data *data = dev->data;

	if (chan == SENSOR_CHAN_PRESS) {
		my_sensor_press_convert(val, data->sample_press);
	} else if (chan == SENSOR_CHAN_AMBIENT_TEMP) {
		my_sensor_temp_convert(val, data->sample_temp);
	} else {
		return -ENOTSUP;
	}

	return 0;
}

static const struct sensor_driver_api my_sensor_api_funcs = {
	.sample_fetch = my_sensor_sample_fetch,
	.channel_get = my_sensor_channel_get,
};

static int my_sensor_init_chip(const struct device *dev)
{
	struct my_sensor_data *data = dev->data;
	const struct my_sensor_config *config = dev->config;
	uint8_t chip_id;

	my_sensor_power_ctrl(dev, 0);
	k_busy_wait(USEC_PER_MSEC * 50U);

	if (my_sensor_power_ctrl(dev, 1) < 0) {
		LOG_DBG("failed to power on device");
		return -EIO;
	}

	k_busy_wait(USEC_PER_MSEC * 20U);

	if (i2c_reg_read_byte(data->i2c_master, config->i2c_slave_addr,
			      MY_SENSOR_REG_WHO_AM_I, &chip_id) < 0) {
		LOG_DBG("failed reading chip id");
		goto err_poweroff;
	}
	if (chip_id != MY_SENSOR_VAL_WHO_AM_I) {
		LOG_DBG("invalid chip id 0x%x", chip_id);
		goto err_poweroff;
	}

	LOG_DBG("chip id 0x%x", chip_id);

	if (my_sensor_set_odr_raw(dev, MY_SENSOR_DEFAULT_SAMPLING_RATE)
				< 0) {
		LOG_DBG("failed to set sampling rate");
		goto err_poweroff;
	}

	if (i2c_reg_update_byte(data->i2c_master, config->i2c_slave_addr,
				MY_SENSOR_REG_CTRL_REG1,
				MY_SENSOR_MASK_CTRL_REG1_BDU,
				(1 << MY_SENSOR_SHIFT_CTRL_REG1_BDU)) < 0) {
		LOG_DBG("failed to set BDU");
		goto err_poweroff;
	}

	return 0;

err_poweroff:
	my_sensor_power_ctrl(dev, 0);
	return -EIO;
}

static int my_sensor_init(const struct device *dev)
{
	const struct my_sensor_config * const config = dev->config;
	struct my_sensor_data *data = dev->data;

	LOG_DBG("MY CUSTOM SENSOR");

	data->i2c_master = device_get_binding(config->i2c_master_dev_name);
	if (!data->i2c_master) {
		LOG_DBG("i2c master not found: %s",
			    config->i2c_master_dev_name);
		return -EINVAL;
	}

	if (my_sensor_init_chip(dev) < 0) {
		LOG_DBG("failed to initialize chip");
		return -EIO;
	}

	return 0;
}

static const struct my_sensor_config my_sensor_config = {
	.i2c_master_dev_name = DT_INST_BUS_LABEL(0),
	.i2c_slave_addr = DT_INST_REG_ADDR(0),
};

static struct my_sensor_data my_sensor_data;

DEVICE_DT_INST_DEFINE(0, my_sensor_init, NULL,
		    &my_sensor_data, &my_sensor_config, POST_KERNEL,
		    CONFIG_SENSOR_INIT_PRIORITY, &my_sensor_api_funcs);
