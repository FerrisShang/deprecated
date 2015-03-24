#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h> 
#include <linux/platform_device.h> 
#include "pdata_def.h"
MODULE_LICENSE("GPL");

static int m_probe(struct platform_device *pdev)
{
	struct pdata *pdata = (struct pdata*)pdev->dev.platform_data;
	printk("d1=%d\nd2=%s\n",
			pdata->data1, pdata->data2);
	printk("m_probe executed..\n");

	return 0;
}
static int m_remove(struct platform_device *dev)
{
	printk("m_remove executed..\n"); 
	return 0;
}
static int m_suspend(struct platform_device *dev, pm_message_t state)
{
	printk("m_suspend executed..\n"); 
	return 0;
}
static int m_resume(struct platform_device *dev)
{
	printk("m_resume executed..\n"); 
	return 0;
}
static void m_shutdown(struct platform_device *dev)
{
	printk("m_shutdown executed..\n"); 
}

static struct platform_driver myDriver = 
{
	.probe = m_probe,
	.remove = m_remove,
	.shutdown = m_shutdown,
	.suspend = m_suspend,
	.resume = m_resume,
	.driver = {
		.name = "plat_test",
		.owner = THIS_MODULE,
	},
};

static int __init plat_init(void)
{
	return platform_driver_register(&myDriver);
}

static void __exit plat_exit(void)
{
	platform_driver_unregister(&myDriver);
}

module_init(plat_init);
module_exit(plat_exit);
