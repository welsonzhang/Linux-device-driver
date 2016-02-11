#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/poll.h>

#define DEVICE_NAME "signal"
#define MAX_BUFFER_SIZE 100

static char buffer[MAX_BUFFER_SIZE];
static int buffer_char_count=0;

static wait_queue_head_t my_queue;

struct semaphore sem;

static struct fasync_struct *fasync;

static ssize_t signal_read(struct file *file,char __user *buf,size_t count,loff_t *ppos)
{
  ssize_t ret=0;
  if(buffer_char_count > 0)
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
   
   if(fasync)
   {
    kill_fasync(&fasync,SIGIO,POLL_OUT);
   }

   up(&sem);
  
   return ret;
  }
  else
  {
     return 0;
  }

  return ret;
}

static ssize_t signal_write(struct file *file,const char __user *buf,size_t count,loff_t *ppos)
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

  if(fasync)
  {
    kill_fasync(&fasync,SIGIO,POLL_IN);
  }
  
  ret=count;
  buffer_char_count=count;

  up(&sem);
  return ret;

}

unsigned int signal_poll(struct file *filp,struct poll_table_struct *wait)
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

int signal_fasync(int fd,struct file *filp,int mode)
{
  return fasync_helper(fd,filp,mode,&fasync);
}

static struct file_operations dev_fops=
{
 .owner=THIS_MODULE,
 .read=signal_read,
 .write=signal_write,
 .poll=signal_poll,
 .fasync=signal_fasync
};

static struct miscdevice misc=
{
 .minor=MISC_DYNAMIC_MINOR,
 .name=DEVICE_NAME,
 .fops=&dev_fops
};

static int __init signal_init()
{
  int ret=misc_register(&misc);
  init_waitqueue_head(&my_queue);
  sema_init(&sem,1);
  printk("signal init success\n");
  return ret;
}

static void __exit signal_exit()
{
  printk("signal exit success\n");
  misc_deregister(&misc);
}

module_init(signal_init);
module_exit(signal_exit);
MODULE_LICENSE("GPL");
