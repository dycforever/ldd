#include <linux/init.h>
#include <linux/modules.h>
#include "add_sub.h"
static long a=1;
static long b=1;
static int addOrSub=1;

static int test_init(){
	lont r ;
	printk(KERN_ALERT "test init\n");
	if(1==addOrSub)
		r = add_integer(a,b);
	else
		r = sub_integer(a,b);
	printk(KERN_ALERT "the result is %ld\n",result);
	return 0;
}

module_init(test_init);
module_exit(test_exit);
module_param(a,long,S_IRUGO);
module_param(b,long,S_IRUGO);
module_param(addOrSub,int,S_IRUGO);

MODULES_LICENSE("Dual BSD/GPL");
