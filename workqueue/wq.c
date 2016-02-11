#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

static struct workqueue_struct *my_wq;

struct work_struct_data
{
  struct work_struct my_work;
  void *data;
}*wsdata;

static void work_handler(struct work_struct *work)
{
 struct work_struct_data *wsdata=(struct work_struct_data *)work;
 printk(KERN_ALERT "work_handler data:%s\n",(char *)wsdata->data);
}

static int __init wq_init(void)
{
  int ret=0;
  my_wq=create_workqueue("my_queue");
  if(my_wq)
  {
    wsdata=(struct work_struct_data *)kmalloc(sizeof(struct work_struct_data),GFP_KERNEL);
    wsdata->data=(char *)"hello world!";
    if(wsdata)
    {
     INIT_WORK(&wsdata->my_work,work_handler);
     ret=queue_work(my_wq,&wsdata->my_work);
    }
  }
  printk(KERN_ALERT "my workqueue init success.\n");
  return ret;
}

static void __exit wq_exit(void)
{
  flush_workqueue(my_wq);
  destroy_workqueue(my_wq);
  printk(KERN_ALERT "my workqueue exit successs.\n");
}

MODULE_LICENSE("GPL");
module_init(wq_init);
module_exit(wq_exit);
