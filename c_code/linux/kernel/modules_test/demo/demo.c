#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

static int __init m_init(void)
{
	printk(KERN_ALERT"module_init..\n");
	return 0;
}
static void __exit m_exit(void)
{
	printk(KERN_ALERT"module_exit..\n");
}
module_init(m_init);
module_exit(m_exit);

