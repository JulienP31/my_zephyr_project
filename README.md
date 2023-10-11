# training

------------------------------------------------------

https://docs.zephyrproject.org/latest/application/index.html
https://docs.zephyrproject.org/latest/guides/porting/board_porting.html
https://docs.zephyrproject.org/latest/reference/drivers/index.html

https://docs.zephyrproject.org/latest/guides/modules.html
https://docs.zephyrproject.org/latest/reference/usermode/syscalls.html

https://docs.zephyrproject.org/latest/guides/dts/intro.html
https://docs.zephyrproject.org/latest/guides/dts/bindings.html
https://docs.zephyrproject.org/apidoc/latest/group__devicetree-generic-id.html
https://docs.zephyrproject.org/apidoc/latest/group__devicetree-gpio.html

https://docs.zephyrproject.org/latest/build/dts/api-usage.html#dt-from-c
https://docs.zephyrproject.org/latest/build/dts/howtos.html#dt-get-device

https://docs.zephyrproject.org/latest/kernel/services/threads/index.html


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

------------------------------------------------------

source ~/Projects/zephyr/zp/.venv/bin/activate
deactivate

source ../zp/zephyr/zephyr-env.sh

west boards

west build -p always -b nucleo_f103rb samples/basic/blinky
west build -p always -b nrf5340dk_nrf5340_cpuapp samples/basic/blinky


