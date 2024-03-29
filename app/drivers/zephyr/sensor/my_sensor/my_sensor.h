#ifndef _MY_SENSOR_H_
#define _MY_SENSOR_H_


#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>


/* -------------------- registers -------------------- */
#define MY_SENSOR_REG_WHO_AM_I     0x0f
#define MY_SENSOR_VAL_WHO_AM_I     0xbd

#define MY_SENSOR_REG_CTRL_REG1    0x20 //< Power up-down + Output data rate
#define MY_SENSOR_REG_CTRL_REG3    0x22 //< INT_DRDY active low
#define MY_SENSOR_REG_CTRL_REG4    0x23 //< DRDY signal on INT_DRDY pin

#define MY_SENSOR_REG_INT_SOURCE   0x25 //< INT_DRDY acknowledgment

#define MY_SENSOR_REG_PRESS_OUT_XL 0x28 //< Pressure low
#define MY_SENSOR_REG_PRESS_OUT_L  0x29 //< Pressure medium
#define MY_SENSOR_REG_PRESS_OUT_H  0x2a //< Pressure high
#define MY_SENSOR_REG_PRESS_OUT    (MY_SENSOR_REG_PRESS_OUT_XL | 0x80) //< MSB for address auto-increment

#define MY_SENSOR_PRESS_DATA_SIZE  3


#if CONFIG_MY_SENSOR_SAMPLING_RATE == 1
	#define MY_SENSOR_DEFAULT_SAMPLING_RATE 1
#elif CONFIG_MY_SENSOR_SAMPLING_RATE == 7
	#define MY_SENSOR_DEFAULT_SAMPLING_RATE 2
#elif CONFIG_MY_SENSOR_SAMPLING_RATE == 13
	#define MY_SENSOR_DEFAULT_SAMPLING_RATE 3
#elif CONFIG_MY_SENSOR_SAMPLING_RATE == 25
	#define MY_SENSOR_DEFAULT_SAMPLING_RATE 4
#endif


/* -------------------- struct -------------------- */
struct my_sensor_config {
	char *i2c_master_dev_name;
	uint16_t i2c_slave_addr;
};


#define MY_SENSOR_THREAD_STACK_SIZE 1024

struct my_sensor_data {
	const struct device *i2c_master;
	
	const struct device *dev; /* [NOTA] Link to device instance */
	
	const struct gpio_dt_spec gpio_spec;
	struct gpio_callback gpio_cb;
	struct sensor_trigger data_ready_trigger;
	sensor_trigger_handler_t data_ready_handler;
	
	K_KERNEL_STACK_MEMBER(irq_thread_stack, MY_SENSOR_THREAD_STACK_SIZE);
	struct k_thread irq_thread;
	struct k_sem irq_sem;
	
	int32_t sample_press;
};


#endif /* _MY_SENSOR_H_ */

