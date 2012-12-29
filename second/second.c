#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>

#include <linux/semaphore.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>

#include <linux/tty_driver.h>
#define GLOBALMEM_SIZE 0x1000
#define MEM_CLEAR 0x1
#define GLOBALMEM_MAJOR 250

static int second_major = 0;

struct second_dev{
	struct cdev cdev;
	atomic_t counter;
	struct timer_list s_timer;
};
struct second_dev *second_devp;
static void second_setup_cdev ( struct second_dev *dev,int index);
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  globalmem_fasync
 *  Description:  
 * =====================================================================================
 */
		static void 
second_timer_handler(unsigned long arg){
		mod_timer(&second_devp->s_timer,jiffies + HZ);
		atomic_inc(&second_devp->counter);
		printk(KERN_NOTICE "current jiffies is %ld\n",jiffies);
}		

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  globalmem_open
 *  Description:  
 * =====================================================================================
 */
		int
second_open ( struct inode *inode, struct file *filp )
{
		init_timer(&second_devp->s_timer);
		second_devp->s_timer.function = &second_timer_handler;
		second_devp->s_timer.expires = jiffies + HZ;
		add_timer(&second_devp->s_timer);
		atomic_set(&second_devp->counter,0);
		return 0;
}		

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  globalmem_release
 *  Description:  
 * =====================================================================================
 */
		int
second_release ( struct inode *inode, struct file *filp)
{
		del_timer(&second_devp->s_timer);
		return 0;
}		

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  globalmem_init
 *  Description:  
 * =====================================================================================
 */
	int
 second_init(  )
{
	int result;
	dev_t devno = MKDEV(second_major, 0);
	
	if(second_major)
			result = register_chrdev_region(devno, 1, "second");
	else{
			result = alloc_chrdev_region(&devno, 0, 1, "second");
			second_major = MAJOR(devno);
	}
	if(result < 0)
			return result;
	second_devp = kmalloc(sizeof(struct second_dev), GFP_KERNEL);
	if(!second_devp){
			result = - ENOMEM;
			goto fail_malloc;
	}
	memset(second_devp, 0, sizeof(struct second_dev));
	second_setup_cdev(second_devp, 0);
//	globalmem_setup_cdev(&globalmem_devp[1], 1);

	//	this macro is deprecated
//	init_MUTEX(&globalmem_devp->sem);
	return 0;

fail_malloc:
	unregister_chrdev_region(devno, 1);
	return result;
}		

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  globalmem_exit(void)
 *  Description:  
 * =====================================================================================
 */
	void
second_exit ( void )
{
	cdev_del(&second_devp->cdev);
	kfree(second_devp);
	unregister_chrdev_region(MKDEV(second_major,0),1);
}				   


		static ssize_t
second_read ( struct file *filp, char __user *buf, size_t size, loff_t * ppos )
{
		int counter = 0;
		counter = atomic_read(&second_devp->counter);
		if(put_user(counter,(int*)buf))
				return -EFAULT;
		else
				return sizeof(unsigned int);
}
		

static const struct file_operations second_fops = {
		.owner = THIS_MODULE,
		.read = second_read,
		.open = second_open,
		.release = second_release,
};


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  globalmem_setup_cdev
 *  Description: actually init() and add() 
 * =====================================================================================
 */
		static void 
second_setup_cdev ( struct second_dev *dev,int index)
{
		int err, devno = MKDEV(second_major, 0);
		cdev_init(&dev->cdev, &second_fops);
		dev->cdev.owner = THIS_MODULE;
		err = cdev_add(&dev->cdev, devno, 1);
		if(err)
				printk(KERN_NOTICE "Error %d adding LED%d",err,index);	
}		


MODULE_AUTHOR("dycforever");
MODULE_LICENSE("Dual BSD/GPL");

module_param(second_major,int,S_IRUGO);
module_init(second_init);
module_exit(second_exit);
