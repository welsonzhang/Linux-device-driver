#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <asm/atomic.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <asm/param.h>

#define DEVICE_NAME "timer_demo"

struct timer_dev
{
  atomic_t counter;
  struct timer_list s_timer;
};

struct timer_dev *timer_devp;

static void timer_demo_handle(unsigned long arg)
{
  mod_timer(&timer_devp->s_timer,jiffies+HZ);
  atomic_inc(&timer_devp->counter);

  printk(KERN_NOTICE "current jiffies is %ld\n",jiffies);
}

int timer_demo_open(struct inode *inode,struct file *filp)
{
  init_timer(&timer_devp->s_timer);
  timer_devp->s_timer.function=&timer_demo_handle;

  timer_devp->s_timer.expires =jiffies+HZ;

  add_timer(&timer_devp->s_timer);

  atomic_set(&timer_devp->counter,0);

  return 0;
}

int timer_demo_release(struct inode *inode,struct file *filp)
{
  del_timer_sync(&timer_devp->s_timer);
  return 0;
}

static ssize_t timer_demo_read(struct file *filp,char __user *buf,size_t count,loff_t *ppos)
{
  int counter;
  counter=atomic_read(&timer_devp->counter);

  if(put_user(counter,(int*)buf))
  {
    return -EFAULT;
  }
  else
  {
    return sizeof(unsigned int);
  }
}

static const struct file_operations timer_fops=
{
 .owner=THIS_MODULE,
 .open=timer_demo_open,
 .release=timer_demo_release,
 .read=timer_demo_read
};

static struct miscdevice misc=
{
 .minor=MISC_DYNAMIC_MINOR,
 .name=DEVICE_NAME,
 .fops=&timer_fops
};

int __init timer_demo_init(void)
{
 int ret=misc_register(&misc);
 
 timer_devp=kmalloc(sizeof(struct timer_dev),GFP_KERNEL);

 if(!timer_devp)
 {
   misc_deregister(&misc);
   return -ENOMEM;
 }
 else
 {
   memset(timer_devp,0,sizeof(struct timer_dev));
 }
 return ret;
}

void __exit timer_demo_exit(void)
{
 kfree(timer_devp);
 misc_deregister(&misc);
}

MODULE_LICENSE("GPL");
module_init(timer_demo_init);
module_exit(timer_demo_exit);
