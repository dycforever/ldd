#include <linux/init.h>
#include <linux/module.h>
#include "add_sub.h"
long add_integer(int a,int b){
	return a+b;
}
long sub_integer(int a,int b){
	return a-b;
}
EXPORT_SYMBOL(add_integer);
EXPORT_SYMBOL(sub_integer);
MODULES_LICENSE("Dual BSD/GPL");
