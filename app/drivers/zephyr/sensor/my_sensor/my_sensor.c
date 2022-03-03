#include <devicetree.h>
#include <drivers/sensor.h>
#include <logging/log.h>

#include "my_sensor.h"


#define DT_DRV_COMPAT st_my_sensor_press


LOG_MODULE_REGISTER(MY_SENSOR, CONFIG_SENSOR_LOG_LEVEL);


/* -------------------- my_sensor_gpio_callback -------------------- */
static void my_sensor_gpio_callback(const struct device *port, struct gpio_callback *cb, uint32_t pins)
{
	struct my_sensor_data *drv_data = CONTAINER_OF(cb, struct my_sensor_data, gpio_cb);
	
	// Temporarily disable IRQ
	gpio_pin_interrupt_configure_dt(&drv_data->gpio_spec, GPIO_INT_DISABLE);
	
	// Give semaphore to thread
	k_sem_give(&drv_data->irq_sem);
}


/* -------------------- my_sensor_thread -------------------- */
static void my_sensor_thread(struct my_sensor_data *drv_data)
{
	while (1) {
		// Take semaphore from IRQ
		k_sem_take(&drv_data->irq_sem, K_FOREVER);
		
		// Call DRDY handler
		if (drv_data->data_ready_handler != NULL) {
			drv_data->data_ready_handler(drv_data->dev, &drv_data->data_ready_trigger);
		}
		
		// Re-enable IRQ
		gpio_pin_interrupt_configure_dt(&drv_data->gpio_spec, GPIO_INT_EDGE_TO_ACTIVE);
	}
}


/* -------------------- my_sensor_trigger_set -------------------- */
static int my_sensor_trigger_set(const struct device *dev, const struct sensor_trigger *trig, sensor_trigger_handler_t handler)
{
	struct my_sensor_data *drv_data = dev->data;
	const struct my_sensor_config *drv_config = dev->config;
	uint8_t out[MY_SENSOR_PRESS_DATA_SIZE] = {0};
	uint8_t byte = 0;
	
	__ASSERT_NO_MSG(trig->type == SENSOR_TRIG_DATA_READY);
	__ASSERT_NO_MSG(trig->chan == SENSOR_CHAN_PRESS);
	__ASSERT_NO_MSG(handler != NULL);
	
	// Clear DRDY line by reading sample data
	if (i2c_burst_read(drv_data->i2c_master, drv_config->i2c_slave_addr, MY_SENSOR_REG_PRESS_OUT, out, MY_SENSOR_PRESS_DATA_SIZE) < 0) {
		LOG_DBG("Failed to clear DRDY line");
		return -EIO;
	}
	
	// Configure IRQ GPIO
	if ( !device_is_ready(drv_data->gpio_spec.port) ) {
		LOG_DBG("IRQ GPIO %s not ready", drv_data->gpio_spec.port->name);
		return -EINVAL;
	}
	
	gpio_pin_configure_dt(&drv_data->gpio_spec, GPIO_INPUT | DT_INST_GPIO_FLAGS(0, irq_gpios));
	
	// Disable IRQ
	gpio_pin_interrupt_configure_dt(&drv_data->gpio_spec, GPIO_INT_DISABLE);
	
	// Power OFF chip
	if (i2c_reg_write_byte(drv_data->i2c_master, drv_config->i2c_slave_addr, MY_SENSOR_REG_CTRL_REG1, 0x00) < 0) {
		LOG_DBG("Failed to power OFF chip");
		return -EIO;
	}
	
	// Configure chip interrupt (DRDY signal on INT_DRDY pin)
	if (i2c_reg_write_byte(drv_data->i2c_master, drv_config->i2c_slave_addr, MY_SENSOR_REG_CTRL_REG4, 0x01) < 0) {
		LOG_DBG("Failed to configure chip interrupt");
		return -EIO;
	}
	
	// Configure IRQ
	drv_data->data_ready_handler = handler;
	drv_data->data_ready_trigger = *trig;
	
	gpio_pin_interrupt_configure_dt(&drv_data->gpio_spec, GPIO_INT_EDGE_TO_ACTIVE);
	
	gpio_init_callback(&drv_data->gpio_cb, my_sensor_gpio_callback, BIT(drv_data->gpio_spec.pin));
	
	if (gpio_add_callback(drv_data->gpio_spec.port, &drv_data->gpio_cb) < 0) {
		LOG_DBG("Could not set gpio callback");
		return -EIO;
	}
	
	// Power ON chip (ODR)
	byte = (1 << 7) | ( (MY_SENSOR_DEFAULT_SAMPLING_RATE & 0x07) << 4 );
	if (i2c_reg_write_byte(drv_data->i2c_master, drv_config->i2c_slave_addr, MY_SENSOR_REG_CTRL_REG1, byte) < 0) {
		LOG_DBG("Failed to power ON chip");
		return -EIO;
	}
	
	return 0;
}


/* -------------------- my_sensor_sample_fetch -------------------- */
static int my_sensor_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	struct my_sensor_data *drv_data = dev->data;
	const struct my_sensor_config *drv_config = dev->config;
	uint8_t out[MY_SENSOR_PRESS_DATA_SIZE] = {0};
	uint8_t byte = 0;
	
	__ASSERT_NO_MSG(chan == SENSOR_CHAN_PRESS);
	
	// Read sample
	if (i2c_burst_read(drv_data->i2c_master, drv_config->i2c_slave_addr, MY_SENSOR_REG_PRESS_OUT, out, MY_SENSOR_PRESS_DATA_SIZE) < 0) {
		LOG_DBG("Failed to read sample");
		return -EIO;
	}
	
	drv_data->sample_press = (int32_t)(  ((uint32_t)(out[2]) << 16) | ((uint32_t)(out[1]) << 8) | out[0]  );
	
	// Acknowledge IRQ
	if (i2c_reg_read_byte(drv_data->i2c_master, drv_config->i2c_slave_addr, MY_SENSOR_REG_INT_SOURCE, &byte) < 0) {
		LOG_DBG("Failed to acknowledge IRQ");
		return -EIO;
	}
	
	return 0;
}


/* -------------------- my_sensor_channel_get -------------------- */
static int my_sensor_channel_get(const struct device *dev, enum sensor_channel chan, struct sensor_value *val)
{
	struct my_sensor_data *drv_data = dev->data;
	
	__ASSERT_NO_MSG(chan == SENSOR_CHAN_PRESS);
	
	val->val1 = drv_data->sample_press / 40960;
	val->val2 = ((int32_t)drv_data->sample_press * 1000000 / 40960) % 1000000;
	
	return 0;
}


/* -------------------- my_sensor_init -------------------- */
static int my_sensor_init(const struct device *dev)
{
	const struct my_sensor_config * const drv_config = dev->config;
	struct my_sensor_data *drv_data = dev->data;
	uint8_t chip_id = 0;
	
	// Get I2C master device
	drv_data->i2c_master = device_get_binding(drv_config->i2c_master_dev_name);
	if (!drv_data->i2c_master) {
		LOG_DBG("i2c master not found: %s", drv_config->i2c_master_dev_name);
		return -EINVAL;
	}
	
	// Check chip ID
	if (i2c_reg_read_byte(drv_data->i2c_master, drv_config->i2c_slave_addr, MY_SENSOR_REG_WHO_AM_I, &chip_id) < 0) {
		LOG_DBG("Failed to read chip id");
		return -EIO;
	}
	
	if (chip_id != MY_SENSOR_VAL_WHO_AM_I) {
		LOG_DBG("Invalid chip id 0x%x", chip_id);
		return -EINVAL;
	}
	
	// Prepare IRQ thread
	drv_data->dev = dev;
	
	k_sem_init(&drv_data->irq_sem, 0, K_SEM_MAX_LIMIT);
	
	k_thread_create(&drv_data->irq_thread, drv_data->irq_thread_stack, MY_SENSOR_THREAD_STACK_SIZE,
			(k_thread_entry_t)my_sensor_thread, drv_data,
			NULL, NULL, K_PRIO_COOP(10), 0, K_NO_WAIT);
	
	return 0;
}


/* -------------------- sensor_driver_api -------------------- */
static const struct sensor_driver_api my_sensor_api_funcs = {
	.trigger_set  = my_sensor_trigger_set,
	.sample_fetch = my_sensor_sample_fetch,
	.channel_get  = my_sensor_channel_get,
};


/* -------------------- struct -------------------- */
static const struct my_sensor_config my_inst_config = {
	.i2c_master_dev_name = DT_INST_BUS_LABEL(0),
	.i2c_slave_addr = DT_INST_REG_ADDR(0),
};


static struct my_sensor_data my_inst_data = {
	.gpio_spec = GPIO_DT_SPEC_INST_GET(0, irq_gpios),
};


DEVICE_DT_INST_DEFINE(0, my_sensor_init, NULL,
			&my_inst_data, &my_inst_config, POST_KERNEL,
			CONFIG_SENSOR_INIT_PRIORITY, &my_sensor_api_funcs);

