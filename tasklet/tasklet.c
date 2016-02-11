#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
//dmesg
static struct tasklet_struct my_tasklet;

static void tasklet_handler(unsigned long data)
{
  printk(KERN_ALERT "tasklet_handler is running.\n");

}

static int __init my_tasklet_init()
{
  tasklet_init(&my_tasklet,tasklet_handler,0);

  tasklet_schedule(&my_tasklet);
  printk(KERN_ALERT "tasklet_init.\n");

  return 0;
}

static void __exit my_tasklet_exit()
{
  tasklet_kill(&my_tasklet);
  printk(KERN_ALERT "tasklet_exit.\n");
}

MODULE_LICENSE("GPL");
module_init(my_tasklet_init);
module_exit(my_tasklet_exit);
