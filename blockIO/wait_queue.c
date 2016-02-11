#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/sched.h>

#define DEVICE_NAME "wq"
#define MAX_BUFFER_SIZE 20

static int buffer_char_count=0;

static char buffer[MAX_BUFFER_SIZE];

static wait_queue_head_t my_queue;

static ssize_t demo_read(struct file *file,char __user *buf,size_t count,loff_t *ppos)
{
  ssize_t ret=0;
  
  if(buffer_char_count >0)
  {
    if(copy_to_user(buf,buffer,buffer_char_count))
    {
      return -EINVAL;
    }

    wake_up_interruptible(&my_queue);
    ret=buffer_char_count;
    
    buffer_char_count=0;
    return ret;
    
  }
  else
  {
    return 0;
  }

  return ret;
}

static ssize_t demo_write(struct file *file,const char __user *buf,size_t count,loff_t *ppos)
{
  ssize_t ret=0;
  
  wait_event_interruptible(my_queue,buffer_char_count==0);
  if(copy_from_user(buffer,buf,count))
  {
   return -EINVAL;
  }
  ret=count;
  buffer_char_count=ret;
  return ret;
}

static struct file_operations dev_fops=
{
 .owner=THIS_MODULE,
 .read=demo_read,
 .write=demo_write
};

static struct miscdevice misc=
{
 .minor=MISC_DYNAMIC_MINOR,
 .name=DEVICE_NAME,
 .fops=&dev_fops
};

static int __init demo_init(void)
{
  int ret=misc_register(&misc);
  init_waitqueue_head(&my_queue);
  printk("demo_init_success\n");
  return ret;
}

static void __exit demo_exit()
{
  printk("demo_exit_success\n");
  misc_deregister(&misc);
}

module_init(demo_init);
module_exit(demo_exit);

MODULE_LICENSE("GPL");

