#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h> 
#include <linux/platform_device.h> 
#include "pdata_def.h"
MODULE_LICENSE("GPL");
static void my_release(struct device *dev)
{
	return;
}
struct pdata pdata = {
	.data1 = 1,
	.data2 = "hello world\n",
};
struct platform_device platform_test = {
	.name = "plat_test",
	.id = -1,
	.dev = {
		.release = my_release,
		.platform_data = &pdata,
	},
};

static int __init plat_init(void)
{
	return platform_device_register(&platform_test);
}

static void plat_exit(void)
{
	platform_device_unregister(&platform_test);
}

module_init(plat_init);
module_exit(plat_exit);
