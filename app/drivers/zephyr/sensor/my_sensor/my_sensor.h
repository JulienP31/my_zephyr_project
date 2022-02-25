/* my_sensor.h - header file for MY_SENSOR pressure and temperature
 * sensor driver
 */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_MY_SENSOR_MY_SENSOR_H_
#define ZEPHYR_DRIVERS_SENSOR_MY_SENSOR_MY_SENSOR_H_

#include <zephyr/types.h>
#include <drivers/i2c.h>
#include <sys/util.h>

#define MY_SENSOR_REG_WHO_AM_I                    0x0F
#define MY_SENSOR_VAL_WHO_AM_I                    0xBD

#define MY_SENSOR_REG_REF_P_XL                    0x08
#define MY_SENSOR_REG_REF_P_L                     0x09
#define MY_SENSOR_REG_REF_P_H                     0x0A

#define MY_SENSOR_REG_RES_CONF                    0x10
#define MY_SENSOR_MASK_RES_CONF_AVGT              (BIT(3) | BIT(2))
#define MY_SENSOR_SHIFT_RES_CONF_AVGT             2
#define MY_SENSOR_MASK_RES_CONF_AVGP              (BIT(1) | BIT(0))
#define MY_SENSOR_SHIFT_RES_CONF_AVGP             0

#define MY_SENSOR_REG_CTRL_REG1                   0x20
#define MY_SENSOR_MASK_CTRL_REG1_PD               BIT(7)
#define MY_SENSOR_SHIFT_CTRL_REG1_PD              7
#define MY_SENSOR_MASK_CTRL_REG1_ODR              (BIT(6) | BIT(5) | BIT(4))
#define MY_SENSOR_SHIFT_CTRL_REG1_ODR             4
#define MY_SENSOR_MASK_CTRL_REG1_DIFF_EN          BIT(3)
#define MY_SENSOR_SHIFT_CTRL_REG1_DIFF_EN         3
#define MY_SENSOR_MASK_CTRL_REG1_BDU              BIT(2)
#define MY_SENSOR_SHIFT_CTRL_REG1_BDU             2
#define MY_SENSOR_MASK_CTRL_REG1_RESET_AZ         BIT(1)
#define MY_SENSOR_SHIFT_CTRL_REG1_RESET_AZ        1
#define MY_SENSOR_MASK_CTRL_REG1_SIM              BIT(0)
#define MY_SENSOR_SHIFT_CTRL_REG1_SIM             0

#define MY_SENSOR_REG_CTRL_REG2                   0x21
#define MY_SENSOR_MASK_CTRL_REG2_BOOT             BIT(7)
#define MY_SENSOR_SHIFT_CTRL_REG2_BOOT            7
#define MY_SENSOR_MASK_CTRL_REG2_FIFO_EN          BIT(6)
#define MY_SENSOR_SHIFT_CTRL_REG2_FIFO_EN         6
#define MY_SENSOR_MASK_CTRL_REG2_STOP_ON_FTH      BIT(5)
#define MY_SENSOR_SHIFT_CTRL_REG2_STOP_ON_FTH     5
#define MY_SENSOR_MASK_CTRL_REG2_FIFO_MEAN_DEC    BIT(4)
#define MY_SENSOR_SHIFT_CTRL_REG2_FIFO_MEAN_DEC   4
#define MY_SENSOR_MASK_CTRL_REG2_I2C_EN           BIT(3)
#define MY_SENSOR_SHIFT_CTRL_REG2_I2C_EN          3
#define MY_SENSOR_MASK_CTRL_REG2_SWRESET          BIT(2)
#define MY_SENSOR_SHIFT_CTRL_REG2_SWRESET         2
#define MY_SENSOR_MASK_CTRL_REG2_AUTOZERO         BIT(1)
#define MY_SENSOR_SHIFT_CTRL_REG2_AUTOZERO        1
#define MY_SENSOR_MASK_CTRL_REG2_ONE_SHOT         BIT(0)
#define MY_SENSOR_SHIFT_CTRL_REG2_ONE_SHOT        0

#define MY_SENSOR_REG_CTRL_REG3                   0x22
#define MY_SENSOR_MASK_CTRL_REG3_INT_H_L          BIT(7)
#define MY_SENSOR_SHIFT_CTRL_REG3_INT_H_L         7
#define MY_SENSOR_MASK_CTRL_REG3_PP_OD            BIT(6)
#define MY_SENSOR_SHIFT_CTRL_REG3_PP_OD           6
#define MY_SENSOR_MASK_CTRL_REG3_INT_S            (BIT(1) | BIT(0))
#define MY_SENSOR_SHIFT_CTRL_REG_INT_S            0

#define MY_SENSOR_REG_CTRL_REG4                   0x23
#define MY_SENSOR_MASK_CTRL_REG4_F_EMPTY          BIT(3)
#define MY_SENSOR_SHIFT_CTRL_REG4_F_EMPTY         3
#define MY_SENSOR_MASK_CTRL_REG4_F_FTH            BIT(2)
#define MY_SENSOR_SHIFT_CTRL_REG4_F_FTH           2
#define MY_SENSOR_MASK_CTRL_REG4_F_OVR            BIT(1)
#define MY_SENSOR_SHIFT_CTRL_REG4_F_OVR           1
#define MY_SENSOR_MASK_CTRL_REG4_DRDY             BIT(0)
#define MY_SENSOR_SHIFT_CTRL_REG4_DRDY            0

#define MY_SENSOR_REG_INTERRUPT_CFG               0x24
#define MY_SENSOR_MASK_INTERRUPT_CFG_LIR          BIT(2)
#define MY_SENSOR_SHIFT_INTERRUPT_CFG_LIR         2
#define MY_SENSOR_MASK_INTERRUPT_CFG_PL_E         BIT(1)
#define MY_SENSOR_SHIFT_INTERRUPT_CFG_PL_E        1
#define MY_SENSOR_MASK_INTERRUPT_CFG_PH_E         BIT(0)
#define MY_SENSOR_SHIFT_INTERRUPT_CFG_PH_E        0

#define MY_SENSOR_REG_INT_SOURCE                  0x25
#define MY_SENSOR_MASK_INT_SOURCE_IA              BIT(2)
#define MY_SENSOR_SHIFT_INT_SOURCE_IA             2
#define MY_SENSOR_MASK_INT_SOURCE_PL              BIT(1)
#define MY_SENSOR_SHIFT_INT_SOURCE_PL             1
#define MY_SENSOR_MASK_INT_SOURCE_PH              BIT(0)
#define MY_SENSOR_SHIFT_INT_SOURCE_PH             0

#define MY_SENSOR_REG_STATUS_REG                  0x27
#define MY_SENSOR_MASK_STATUS_REG_P_OR            BIT(5)
#define MY_SENSOR_SHIFT_STATUS_REG_P_OR           5
#define MY_SENSOR_MASK_STATUS_REG_T_OR            BIT(4)
#define MY_SENSOR_SHIFT_STATUS_REG_T_OR           4
#define MY_SENSOR_MASK_STATUS_REG_P_DA            BIT(1)
#define MY_SENSOR_SHIFT_STATUS_REG_P_DA           1
#define MY_SENSOR_MASK_STATUS_REG_T_DA            BIT(0)
#define MY_SENSOR_SHIFT_STATUS_REG_T_DA           0

#define MY_SENSOR_REG_PRESS_OUT_XL                0x28
#define MY_SENSOR_REG_PRESS_OUT_L                 0x29
#define MY_SENSOR_REG_PRESS_OUT_H                 0x2A

#define MY_SENSOR_REG_TEMP_OUT_L                  0x2B
#define MY_SENSOR_REG_TEMP_OUT_H                  0x2C

#define MY_SENSOR_REG_FIFO_CTRL                   0x2E
#define MY_SENSOR_MASK_FIFO_CTRL_F_MODE           (BIT(7) | BIT(6) | BIT(5))
#define MY_SENSOR_SHIFT_FIFO_CTRL_F_MODE          5
#define MY_SENSOR_MASK_FIFO_CTRL_WTM_POINT        (BIT(4) | BIT(3) | BIT(2) | \
						 BIT(2) | BIT(1) | BIT(0))
#define MY_SENSOR_SHIFT_FIFO_CTRL_WTM_POINT       0

#define MY_SENSOR_REG_FIFO_STATUS                 0x2F
#define MY_SENSOR_MASK_FIFO_STATUS_FTH_FIFO       BIT(7)
#define MY_SENSOR_SHIFT_FIFO_STATUS_FTH_FIFO      7
#define MY_SENSOR_MASK_FIFO_STATUS_OVR            BIT(6)
#define MY_SENSOR_SHIFT_FIFO_STATUS_OVR           6
#define MY_SENSOR_MASK_FIFO_STATUS_EMPTY_FIFO     BIT(5)
#define MY_SENSOR_SHIFT_FIFO_STATUS_EMPTY_FIFO    5
#define MY_SENSOR_MASK_FIFO_STATUS_FSS            (BIT(4) | BIT(3) | BIT(2) | \
						 BIT(1) | BIT(0))
#define MY_SENSOR_SHIFT_FIFO_STATUS_FSS           0

#define MY_SENSOR_REG_THS_P_L                     0x30
#define MY_SENSOR_REG_THS_P_H                     0x31

#define MY_SENSOR_REG_RPDS_L                      0x39
#define MY_SENSOR_REG_RPDS_H                      0x3A

#if CONFIG_MY_SENSOR_SAMPLING_RATE == 1
	#define MY_SENSOR_DEFAULT_SAMPLING_RATE      1
#elif CONFIG_MY_SENSOR_SAMPLING_RATE == 7
	#define MY_SENSOR_DEFAULT_SAMPLING_RATE      2
#elif CONFIG_MY_SENSOR_SAMPLING_RATE == 13
	#define MY_SENSOR_DEFAULT_SAMPLING_RATE      3
#elif CONFIG_MY_SENSOR_SAMPLING_RATE == 25
	#define MY_SENSOR_DEFAULT_SAMPLING_RATE      4
#endif


struct my_sensor_config {
	char *i2c_master_dev_name;
	uint16_t i2c_slave_addr;
};

struct my_sensor_data {
	const struct device *i2c_master;

	int32_t sample_press;
	int16_t sample_temp;
};

#endif /* ZEPHYR_DRIVERS_SENSOR_MY_SENSOR_MY_SENSOR_H_ */
