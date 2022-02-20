cmake_minimum_required(VERSION 3.20.0)

list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/my_driver)
  
find_package(Zephyr)
project(my_zephyr_app)

target_sources(app PRIVATE src/main.c)

