# my_zephyr_project

------------------------------------------------------

https://docs.zephyrproject.org/latest/application/index.html
https://docs.zephyrproject.org/latest/guides/porting/board_porting.html
https://docs.zephyrproject.org/latest/reference/drivers/index.html

https://docs.zephyrproject.org/latest/guides/modules.html
https://docs.zephyrproject.org/latest/reference/usermode/syscalls.html

https://docs.zephyrproject.org/latest/guides/dts/intro.html
https://docs.zephyrproject.org/apidoc/latest/group__devicetree-generic-id.html
https://docs.zephyrproject.org/apidoc/latest/group__devicetree-gpio.html

https://docs.zephyrproject.org/latest/reference/kernel/threads/index.html


https://github.com/zephyrproject-rtos/example-application


/home/julien/zephyrproject/zephyr/samples/application_development
-> out-of-tree board
-> out-of-tree driver

------------------------------------------------------

picocom -b 115200 /dev/ttyACM0


source ../zephyrproject/zephyr/zephyr-env.sh

west build -s app -b my_board -- -DBOARD_ROOT=..
west flash
west debug

west build -t clean
west build -t pristine

west build -t menuconfig
west build -t guiconfig

