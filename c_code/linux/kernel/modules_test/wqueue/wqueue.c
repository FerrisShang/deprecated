#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/interrupt.h>

static struct workqueue_struct *test_wq;
static struct delayed_work test_dwq;

void delay_func(struct work_struct *work)
{
	static int i=0;
	printk(KERN_INFO "My name is delay_func!i=%d\n",i++);
	queue_delayed_work(test_wq,&test_dwq,100);
}

static int __init example_init(void)
{
	test_wq = create_workqueue("test_wq");
	if (!test_wq) {
		printk(KERN_ERR "No memory for workqueue\n");
		return -1;
	}
	printk("Create Workqueue successful!\n");
	INIT_DELAYED_WORK(&test_dwq, delay_func);
	queue_delayed_work(test_wq, &test_dwq, 0);
	return 0;
}

static void __exit example_exit(void)
{
	destroy_workqueue(test_wq);
	printk(KERN_INFO "Goodbye! delayed_work.\n");
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL");
