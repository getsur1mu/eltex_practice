#include <linux/module.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/kd.h>
#include <linux/console_struct.h>
#include <linux/vt_kern.h>
#include <linux/timer.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#define BLINK_DELAY HZ/5
#define ALL_LEDS_ON 0x07
#define RESTORE_LEDS 0xFF

MODULE_DESCRIPTION("Keyboard leds controlled through sysfs.");
MODULE_AUTHOR("get.sur1mu");
MODULE_LICENSE("GPL");

static struct timer_list my_timer;
static struct tty_driver *my_driver;
static struct kobject *example_kobject;
static int _kbledstatus = 0;
static int test = 3;

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%d\n", test);
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	int value;

	if (sscanf(buf, "%d", &value) != 1)
		return -EINVAL;

	if (value < 0 || value > ALL_LEDS_ON)
		return -EINVAL;

	test = value;
	return count;
}

static struct kobj_attribute foo_attribute = __ATTR(test, 0660, foo_show,
						    foo_store);

static void my_timer_func(struct timer_list *ptr)
{
	int *pstatus = &_kbledstatus;

	if (*pstatus == test)
		*pstatus = RESTORE_LEDS;
	else
		*pstatus = test;

	(my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED,
				*pstatus);
	my_timer.expires = jiffies + BLINK_DELAY;
	add_timer(&my_timer);
}

static int __init kbleds_init(void)
{
	int error = 0;

	printk(KERN_INFO "kbleds: loading\n");

	my_driver = vc_cons[fg_console].d->port.tty->driver;

	example_kobject = kobject_create_and_add("systest", kernel_kobj);
	if (!example_kobject)
		return -ENOMEM;

	error = sysfs_create_file(example_kobject, &foo_attribute.attr);
	if (error)
	{
		kobject_put(example_kobject);
		return error;
	}

	timer_setup(&my_timer, my_timer_func, 0);
	my_timer.expires = jiffies + BLINK_DELAY;
	add_timer(&my_timer);

	return 0;
}

static void __exit kbleds_cleanup(void)
{
	printk(KERN_INFO "kbleds: unloading...\n");
	timer_delete_sync(&my_timer);
	(my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED,
				RESTORE_LEDS);
	kobject_put(example_kobject);
}

module_init(kbleds_init);
module_exit(kbleds_cleanup);
