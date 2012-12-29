#include <linux/init.h>
#include <linux/module.h>
#define MODULE
static int hello_init(){
	printk(KERN_ALERT "Hello, World\n");
	return 0;
}
static void hello_exit(){
	printk(KERN_ALERT "Goodbye, World");
}
module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
