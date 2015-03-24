#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");

static long misc_dev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk("cmd=%d,arg=%ld\n",cmd,arg);
	return 0;
}

static int misc_dev_open(struct inode *inode, struct file *filp)
{
	printk("%s@%d\n",__func__, __LINE__);
	return 0;
}

static int misc_dev_release(struct inode *inode , struct file *filp)
{
	printk("%s@%d\n",__func__, __LINE__);
	return 0;
}

static int misc_dev_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	printk (KERN_INFO "devRead-size:%d.ppos:%lld\n",size,*ppos);
	*ppos += size;
	return size;
}
static int misc_dev_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	printk (KERN_INFO "devWrite-size:%d.ppos:%lld\n",size,*ppos);
	*ppos += size;
	return size;
}
unsigned int misc_dev_poll (struct file *filp, struct poll_table_struct *wait)
{
	printk (KERN_INFO "misc_dev_poll\n");
	return 0;
}
loff_t misc_dev_llseek (struct file *filp, loff_t offset, int CMD)
{
	printk (KERN_INFO "misc_dev_llseek\n");
	return 0;
}
static struct file_operations  misc_dev_fops = {
	.owner		= THIS_MODULE,
	.open		= misc_dev_open,
	.read	    = misc_dev_read,
	.write	    = misc_dev_write,
	.poll	    = misc_dev_poll,
	.llseek	    = misc_dev_llseek,
	.unlocked_ioctl = misc_dev_ioctl,

	.release	= misc_dev_release,
};

static struct miscdevice misc_dev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= "misc_dev",
	.fops		= &misc_dev_fops,
};

static int __init misc_dev_init(void)
{
	printk("misc_register(&misc_dev);\n");
	misc_register(&misc_dev);
	return 0;
}

static void __exit misc_dev_exit(void)
{
	printk("misc_deregister(&misc_dev);\n");
	misc_deregister(&misc_dev);
}
module_init(misc_dev_init);
module_exit(misc_dev_exit);
