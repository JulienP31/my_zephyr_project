#ifndef __MY_DRIVER_H__
#define __MY_DRIVER_H__


#include <device.h>


/* -------------------- Typedefs -------------------- */
typedef int  (*my_driver_do_this_t)(const struct device *dev, int foo, int bar);
typedef void (*my_driver_do_that_t)(const struct device *dev, void *baz);


/* -------------------- Driver API structure -------------------- */
__subsystem struct my_driver_api {
      my_driver_do_this_t do_this;
      my_driver_do_that_t do_that;
};


/* -------------------- Driver API declaration -------------------- */
__syscall int  my_driver_do_this(const struct device *dev, int foo, int bar);
__syscall void my_driver_do_that(const struct device *dev, void *baz);


/* -------------------- Driver API implementation -------------------- */
static inline int z_impl_my_driver_do_this(const struct device *dev, int foo, int bar)
{
      const struct my_driver_api *api = (const struct my_driver_api *)dev->api;

      return api->do_this(dev, foo, bar);
}


static inline void z_impl_my_driver_do_that(const struct device *dev, void *baz)
{
      const struct my_driver_api *api = (const struct my_driver_api *)dev->api;

      api->do_that(dev, baz);
}


/* -------------------- Driver API verification -------------------- */
#ifdef CONFIG_USERSPACE
static inline int z_vrfy_my_driver_do_this(const struct device *dev, int foo, int bar)
{
	// Do some verification...

	return z_impl_my_driver_do_this(dev, foo, bar);
}
#include <syscalls/my_driver_do_this_mrsh.c>


static inline void z_vrfy_my_driver_do_that(const struct device *dev, void *baz)
{
	// Do some verification...

	z_impl_my_driver_do_that(dev, baz);
}
#include <syscalls/my_driver_do_that_mrsh.c>
#endif /* CONFIG_USERSPACE */


#include <syscalls/my_driver.h>


#endif /* __MY_DRIVER_H__ */

