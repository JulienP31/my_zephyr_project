#ifndef _MY_SENSOR_H_
#define _MY_SENSOR_H_


#include <drivers/i2c.h>


/* -------------------- registers -------------------- */
#define MY_SENSOR_REG_WHO_AM_I     0x0f
#define MY_SENSOR_VAL_WHO_AM_I     0xbd

#define MY_SENSOR_REG_CTRL_REG1    0x20 //< Power up-down + Output data rate
#define MY_SENSOR_REG_CTRL_REG2    0x21 //< FIFO mode + Watermark mode
#define MY_SENSOR_REG_CTRL_REG3    0x22 //< INT1 active low
#define MY_SENSOR_REG_CTRL_REG4    0x23 //< Watermark signal on INT1 pin

#define MY_SENSOR_REG_INT_SOURCE   0x25 //< INT1 acknowledgment

#define MY_SENSOR_REG_PRESS_OUT_XL 0x28 //< Pressure low
#define MY_SENSOR_REG_PRESS_OUT_L  0x29 //< Pressure medium
#define MY_SENSOR_REG_PRESS_OUT_H  0x2a //< Pressure high

#define MY_SENSOR_REG_FIFO_CTRL    0x2e //< Stream-Bypass mode (FIFO) + Watermark level


/* -------------------- struct -------------------- */
struct my_sensor_config {
	char *i2c_master_dev_name;
	uint16_t i2c_slave_addr;
};


struct my_sensor_data {
	const struct device *i2c_master;
	int32_t sample_press;
};


#endif /* _MY_SENSOR_H_ */

