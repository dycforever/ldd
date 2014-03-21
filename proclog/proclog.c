#include <linux/init.h>
#include <linux/module.h>

#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>

#include <asm/uaccess.h>	/* copy_*_user */


//#define MODULE


static void *proclog_seq_start(struct seq_file *s, loff_t *pos)
{
    printk(KERN_ALERT "call proclog_seq_start with s[%p], pos[%lld]\n", s, *pos);
    if (*pos != 0) {
        return NULL;
    }
	return pos;
}

static void *proclog_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
//    if (v == NULL) {
//        return NULL;
//    }
    printk(KERN_ALERT "call proclog_seq_next with s[%p], v[%p], pos[%lld]\n", s, v, *pos);
	return NULL;
}

static void proclog_seq_stop(struct seq_file *s, void *v)
{
    printk(KERN_ALERT "call proclog_seq_stop with s[%p], v[%p]\n", s, v);
}

void print_kobj(struct kobject* kobj, char* buf, size_t len) {
    if (kobj == NULL) {
        printk("kobj is NULL");
    }
    memset(buf, 0, len);
    snprintf(buf, len, "name:[%s] parent[%s] kset[%s]", kobj->k_name, 
            kobj->parent==NULL ? "NULL" : kobj->parent->k_name, 
            kobj->kset==NULL ? "NULL" : kobj->kset->kobj.k_name);
}

void print_kset(struct kset* set, char* buf, size_t len) {
    size_t obj_len = 0;
    char* start;

    if (set == NULL) {
        printk("set is NULL");
    }

    memset(buf, 0, len);
    print_kobj(&(set->kobj), buf, len);
    obj_len  = strlen(buf);
    start = buf + obj_len;
    snprintf(start, len-obj_len, " ktype:[%p] ", set->ktype);
}

extern struct kset bus_subsys;
static int proclog_seq_show(struct seq_file *s, void *v)
{
    char* buf = NULL;
    if (v == NULL) {
        return 0;
    }
    buf = kmalloc(1024, GFP_KERNEL);
    print_kset(&bus_subsys, buf, 1024);
	seq_printf(s, "%s\n", buf);
    kfree(buf);
	return 0;
}

static struct seq_operations proclog_seq_ops = {
	.start = proclog_seq_start,
	.next  = proclog_seq_next,
	.stop  = proclog_seq_stop,
	.show  = proclog_seq_show
};

static int proclog_proc_open(struct inode *inode, struct file *file)
{
	printk(KERN_ALERT "call proclog_proc_open\n");
	return seq_open(file, &proclog_seq_ops);
}


static struct file_operations proclog_ops = {
    .owner   = THIS_MODULE,
    .open    = proclog_proc_open,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = seq_release
};

static int proclog_init(void) {
    struct proc_dir_entry* entry;
	printk(KERN_ALERT "Hello, proclog_init\n");
    entry = create_proc_entry("proclog", 0, NULL);
    if (entry) {
        entry->proc_fops = &proclog_ops;
    }
	return 0;
}

static void proclog_exit(void) {
	printk(KERN_ALERT "Goodbye, proclog_exit");
    remove_proc_entry("proclog", NULL);
}

module_init(proclog_init);
module_exit(proclog_exit);

MODULE_LICENSE("GPL");
