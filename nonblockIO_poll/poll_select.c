#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/poll.h>

#define DEVICE_NAME "poll_select"
#define MAX_BUFFER_SIZE 20

static char buffer[MAX_BUFFER_SIZE];
static int buffer_char_count=0;

static wait_queue_head_t my_queue;

struct semaphore sem;

static ssize_t demo_read(struct file *file,char __user *buf,size_t count,loff_t *ppos)
{
  ssize_t ret=0;
  if(buffer_char_count>0)
  {
   if(down_interruptible(&sem))
   {
     return -ERESTARTSYS;
   }
   if(copy_to_user(buf,buffer,buffer_char_count))
   {
     return -EINVAL;
   }

   wake_up_interruptible(&my_queue);
   ret=buffer_char_count;
   buffer_char_count=0;

   up(&sem);
   return ret;
 
  }else
  {
    return 0;
  }

  return 0;
}

static ssize_t demo_write(struct file *file,const char __user *buf,size_t count,loff_t *ppos)
{
  ssize_t ret=0;
  
  if(down_interruptible(&sem)) 
  {
    return -ERESTARTSYS;
  }

  wait_event_interruptible(my_queue,buffer_char_count==0);

  if(copy_from_user(buffer,buf,count))
  {
   return -EINVAL;
  }

  ret=count;
  buffer_char_count=count;

  up(&sem);

  return ret;
}

unsigned int demo_poll(struct file *filp,struct poll_table_struct *wait)
{
  unsigned int mask=0;

  if(down_interruptible(&sem))
  {
    return -ERESTARTSYS;
  }

  poll_wait(filp,&my_queue,wait);

  if(buffer_char_count>0)
  {
   mask|=POLLIN|POLLRDNORM;
  }

  if(buffer_char_count==0)
  {
   mask|=POLLOUT|POLLWRNORM;
  }
  up(&sem);
  return mask;
}

static struct file_operations dev_fops=
{
 .owner = THIS_MODULE,
 .read = demo_read,
 .write = demo_write,
 .poll = demo_poll
};

static struct miscdevice misc=
{
 .minor = MISC_DYNAMIC_MINOR,
 .name = DEVICE_NAME,
 .fops = &dev_fops
};

static int __init demo_init(void)
{
  int ret=misc_register(&misc);

  init_waitqueue_head(&my_queue);

  sema_init(&sem,1);
  printk("demo_init_success\n");
  return ret;
}

static void __exit demo_exit(void)
{
  printk("demo_exit_success\n");
  misc_deregister(&misc);
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");
