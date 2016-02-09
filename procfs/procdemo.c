#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <asm/io.h>
#include <linux/seq_file.h>

#define PROC_NAME_BASE "proc_demo"
#define PROC_NAME "bin2dec"
#define PROC_NAME_READONLY "readonly"

static long decimal;

static struct proc_dir_entry *proc_entry_base=NULL;
static struct proc_dir_entry *proc_entry=NULL;
static struct proc_dir_entry *proc_entry_readonly=NULL;


static int proc_demo_write(struct file *file,const char __user *buffer,unsigned long count,loff_t *pos)
{
  char buf[count];
  int ret;

  if(copy_from_user(buf,buffer,count))
  {
   ret=-EFAULT;
   goto err;
  }
  else
  {
    decimal=simple_strtoul(buf,NULL,2);
    return count; 
  }

err: return ret;
}

static int proc_demo_read(struct seq_file *m, void *v) 
{
  seq_printf(m, "%ld\n",decimal);
  return 0;
}

static int proc_demo_open(struct inode *inode, struct  file *file)
{
  return single_open(file, proc_demo_read, NULL);
}

static int proc_readonly_read(struct seq_file *m,void *v)
{
  seq_printf(m,"Hello proc readonly!\n");
  return 0;
}

static int proc_readonly_open(struct inode *inode,struct file *file)
{
  return single_open(file,proc_readonly_read,NULL);
}

static const struct file_operations proc_file_fops = {
 .owner = THIS_MODULE,
 .open = proc_demo_open,
 .read  = seq_read,
 .write = proc_demo_write,
 .release = single_release
};

static const struct file_operations proc_readonly_file_fops = {
 .owner = THIS_MODULE,
 .open = proc_readonly_open,
 .read  = seq_read,
 .release = single_release
};

static int __init proc_demo_init()
{
  proc_entry_base=proc_mkdir(PROC_NAME_BASE,NULL);

  if(proc_entry_base==NULL)
  {
   printk(KERN_INFO "Couldn't create proc entry dir\n");
   goto err;
  }
  
  proc_entry=proc_create(PROC_NAME, 0666, proc_entry_base, &proc_file_fops); 
  if(proc_entry == NULL)
  {
   goto err;
  }
  proc_entry_readonly=proc_create(PROC_NAME_READONLY,0444,proc_entry_base,&proc_readonly_file_fops);
  if(proc_entry_readonly == NULL)
  {
   goto err;
  } 
  return 0;

err:return -ENOMEM;
}

static void __exit proc_demo_exit(void)
{
  if(proc_entry!=NULL)
  {
   remove_proc_entry(PROC_NAME,proc_entry_base);
  }

  if(proc_entry_readonly!=NULL)
  {
    remove_proc_entry(PROC_NAME_READONLY,proc_entry_base);
  }
 
  if(proc_entry_base!=NULL)
  {
   remove_proc_entry(PROC_NAME_BASE,NULL);
  }
}

module_init(proc_demo_init);
module_exit(proc_demo_exit);
MODULE_LICENSE("GPL");
