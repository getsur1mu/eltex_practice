#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

static int __init hello_init(void)
{
	printk(KERN_INFO "Hello, kernel module is loaded\n");
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "Hello, kernel module is unloaded\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("get.sur1mu");
MODULE_DESCRIPTION("My first simple hello world module");
MODULE_LICENSE("Student Bee License");
