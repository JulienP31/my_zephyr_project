# my_zephyr_project

------------------------------------------------------

https://docs.zephyrproject.org/latest/application/index.html
https://docs.zephyrproject.org/latest/guides/porting/board_porting.html

/home/julien/zephyrproject/zephyr/samples/application_development
-> out-of-tree board
-> out-of-tree driver

------------------------------------------------------

picocom -b 115200 /dev/ttyACM0


source ../zephyrproject/zephyr/zephyr-env.sh

west build -b my_board -- -DBOARD_ROOT=.
west flash
west debug

west build -t clean
west build -t pristine

