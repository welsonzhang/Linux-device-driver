#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>

#define DEVICE_NAME "atomic"

static int atom=0;

static atomic_t int_atomic_available=ATOMIC_INIT(1);

static int atomic_open(struct inode *node,struct file *file)
{
  if(atom)
  {
    if(!atomic_dec_and_test(&int_atomic_available))
    {
      atomic_inc(&int_atomic_available);
      return -EBUSY;
    }
  }
  return 0;
}

static int atomic_release(struct inode *node,struct file *file)
{
  if(atom)
  {
    atomic_inc(&int_atomic_available);
  }
  return 0;
}

static struct file_operations dev_fops=
{
 .owner = THIS_MODULE,
 .open = atomic_open,
 .release = atomic_release
};

static struct miscdevice misc =
{
 .minor = MISC_DYNAMIC_MINOR,
 .name = DEVICE_NAME,
 .fops = &dev_fops
};

static int __init atomic_init()
{
  int ret=misc_register(&misc);
  printk("atomi_init_success\n");
  
  return ret;
}

static void __exit atomic_exit()
{
  printk("atomic_exit_success\n");
  misc_deregister(&misc);
}

module_init(atomic_init);
module_exit(atomic_exit);
module_param(atom,int,S_IRUGO|S_IWUSR);
MODULE_LICENSE("GPL");
