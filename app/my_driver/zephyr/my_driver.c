#include "my_driver.h"
#include <zephyr/types.h>
#include <syscall_handler.h>


static int init(const struct device *dev)
{
	printk("[MY_DRIVER] init\n"); //< POST_KERNEL init level to be specified in DEVICE_DEFINE

	return 0;
}


static int do_this_impl(const struct device *dev, int foo, int bar)
{
	printk("[MY_DRIVER] do_this_impl\n");
	
	return (foo + bar);
}


static void do_that_impl(const struct device *dev, void *baz)
{
	printk("[MY_DRIVER] do_that_impl\n");
}


static struct my_driver_api my_driver_api_funcs = {
      .do_this = do_this_impl,
      .do_that = do_that_impl
};


DEVICE_DEFINE(my_device, "MY_DEVICE",
		    init, NULL, NULL, NULL,
		    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
		    &my_driver_api_funcs);
		    
