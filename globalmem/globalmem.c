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

static int globalmem_major = 0;

struct globalmem_dev{
	struct cdev cdev;
	unsigned int current_len;
	unsigned char mem[GLOBALMEM_SIZE];
	struct semaphore sem;
	wait_queue_head_t r_wait;
	wait_queue_head_t w_wait;
	struct fasync_struct *async_queue;
};

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  globalmem_fasync
 *  Description:  
 * =====================================================================================
 */
		static int
globalmem_fasync ( int fd, struct file* filp, int mode)
{
		struct globalmem_dev *dev = filp->private_data;
		return fasync_helper(fd, filp, mode, &dev->async_queue);
}		
struct globalmem_dev *globalmem_devp;
static void globalmem_setup_cdev ( struct globalmem_dev *dev,int index);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  globalmem_open
 *  Description:  
 * =====================================================================================
 */
		int
globalmem_open ( struct inode *inode, struct file *filp )
{
		struct globalmem_dev *dev;
		dev = container_of(inode->i_cdev,struct globalmem_dev,cdev);
		filp->private_data = dev;
		return 0;
}		

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  globalmem_release
 *  Description:  
 * =====================================================================================
 */
		int
globalmem_release ( struct inode *inode, struct file *filp)
{
		globalmem_fasync(-1,filp,0);
		return 0;
}		

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  globalmem_init
 *  Description:  
 * =====================================================================================
 */
	int
globalmem_init (  )
{
	int result;
	dev_t devno = MKDEV(globalmem_major, 0);
	
	if(globalmem_major)
			result = register_chrdev_region(devno, 1, "globalmem");
	else{
			result = alloc_chrdev_region(&devno, 0, 1, "globalmem");
			globalmem_major = MAJOR(devno);
	}
	if(result < 0)
			return result;
	globalmem_devp = kmalloc(sizeof(struct globalmem_dev), GFP_KERNEL);
	if(!globalmem_devp){
			result = - ENOMEM;
			goto fail_malloc;
	}
	memset(globalmem_devp, 0, sizeof(struct globalmem_dev));
	globalmem_setup_cdev(globalmem_devp, 0);
//	globalmem_setup_cdev(&globalmem_devp[1], 1);

	//	this macro is deprecated
//	init_MUTEX(&globalmem_devp->sem);
	globalmem_devp->async_queue =  kmalloc(sizeof(struct fasync_struct), GFP_KERNEL);
	sema_init(&globalmem_devp->sem,1);
	init_waitqueue_head(&globalmem_devp->r_wait);
	init_waitqueue_head(&globalmem_devp->w_wait);

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
globalmem_exit ( void )
{
	cdev_del(&globalmem_devp->cdev);
	kfree(globalmem_devp);
	unregister_chrdev_region(MKDEV(globalmem_major,0),1);
}				   

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  globalmem_llseek
 *  Description:  
 * =====================================================================================
 */
		static loff_t
globalmem_llseek ( struct file *filp, loff_t offset, int orig )
{
		loff_t ret;
		switch(orig){
		case 0: //seek from the begin
				if(offset < 0){
						ret = -EINVAL;
						break;
				}
				if((unsigned int)offset > GLOBALMEM_SIZE){
						ret = -EINVAL;
						break;
				}
				filp->f_pos = (unsigned int)offset;
				ret = filp->f_pos;
				break;
		case 1: //seek from here
				if( (filp->f_pos + offset) > GLOBALMEM_SIZE){
						ret = -EINVAL;
						break;
				}
				if( (filp->f_pos + offset) < 0){
						ret = -EINVAL;
						break;
				}
				filp->f_pos += offset;
				ret = filp->f_pos;
				break;
		default:
				ret = -EINVAL;
		}
		return ret;
}		

		static ssize_t
globalmem_read ( struct file *filp, char __user *buf, size_t size, loff_t * ppos )
{
		unsigned long p = *ppos;
		unsigned int count = size;
		int ret = 0;
		struct globalmem_dev *dev = filp->private_data;
		DECLARE_WAITQUEUE(wait,current);

		if(p >= GLOBALMEM_SIZE)
				return 0;
		if(count >GLOBALMEM_SIZE -p)
				count = GLOBALMEM_SIZE -p;
		if(down_interruptible(&dev->sem))
			return -ERESTARTSYS;	
		if(copy_to_user(buf, (void*)(dev->mem + p),count))
				ret = -EFAULT;
		else{
				*ppos += count;
				ret = count;
				printk(KERN_INFO "read %d bytes(s) form %ld\n",count,p);
		}
		up(&dev->sem);
		return ret;
}		
		static ssize_t
globalmem_write ( struct file *filp, const char __user *buf,size_t count, loff_t *ppos)
{
		unsigned long p = *ppos;
		int ret = 0;
		struct globalmem_dev *dev = filp->private_data;
//		struct globalmem_dev *dev = globalmem_devp;
		if(p >= GLOBALMEM_SIZE)
				return 0;
		if(count > GLOBALMEM_SIZE -p)
				count = GLOBALMEM_SIZE -p;
		printk(KERN_INFO "begin writting\n");
		if(down_interruptible(&dev->sem))
			return -ERESTARTSYS;	
		if(copy_from_user((void*)(dev->mem + p), buf, count)){
				printk(KERN_INFO "copy %d bytes(s) form %ld\n",count,p);
				ret = -EFAULT;

		}
		else{
				*ppos += count;
				if(dev->async_queue){
						kill_fasync(&dev->async_queue, SIGIO, POLL_IN);
						printk(KERN_INFO "signal send");
				}else 
					printk(KERN_INFO "signal not send");
				ret = count;
				printk(KERN_INFO "written %d bytes(s) form %ld\n",count,p);
		}		
		up(&dev->sem);
		printk(KERN_INFO "over writting %d bytes\n",ret);
		return ret;
}		

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  globalmem_ioctl
 *  Description:  
 * =====================================================================================
 */
		 int
globalmem_ioctl ( struct file* filp,unsigned int cmd,unsigned long arg)
{
		struct globalmem_dev *dev = filp->private_data;
		switch(cmd){
		case MEM_CLEAR:
				memset(dev->mem,0,GLOBALMEM_SIZE);
				printk(KERN_INFO "globalmem is set to zero\n");
				break;
		default:
				return -EINVAL;
		}
		return 0;
}		

static const struct file_operations globalmem_fops = {
		.owner = THIS_MODULE,
		.llseek = globalmem_llseek,
		.read = globalmem_read,
		.write = globalmem_write,
		.unlocked_ioctl = globalmem_ioctl,
		.open = globalmem_open,
		.release = globalmem_release,
		.fasync = globalmem_fasync
};


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  globalmem_setup_cdev
 *  Description: actually init() and add() 
 * =====================================================================================
 */
		static void 
globalmem_setup_cdev ( struct globalmem_dev *dev,int index)
{
		int err, devno = MKDEV(globalmem_major, 0);
		cdev_init(&dev->cdev, &globalmem_fops);
		dev->cdev.owner = THIS_MODULE;
		err = cdev_add(&dev->cdev, devno, 1);
		if(err)
				printk(KERN_NOTICE "Error %d adding globalmem",err);	
}		


MODULE_AUTHOR("dycforever");
MODULE_LICENSE("Dual BSD/GPL");




module_param(globalmem_major,int,S_IRUGO);
module_init(globalmem_init);
module_exit(globalmem_exit);
