if(CONFIG_MY_DRIVER)
  # Add my_driver.h to the set of global include paths.
  zephyr_include_directories(.)

  zephyr_library()
  zephyr_library_sources(my_driver.c)
endif()

