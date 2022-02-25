if(CONFIG_MY_SENSOR)
	zephyr_library()
	zephyr_library_sources(my_sensor.c)
endif()

