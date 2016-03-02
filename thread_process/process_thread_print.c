#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>

static int __init print_init(void)
{
  struct task_struct *task;
  struct task_struct *thread;  
  printk("process info:\n");
  //for_each_process(task)
  do_each_thread(task,thread)
    printk("%s  pid:%d tgid:%d father pid:%d\n",
           thread->comm,
           thread->pid,
           thread->tgid,
           thread->parent->pid);

  while_each_thread(task,thread);
  return 0;
}

static void __exit print_exit(void)
{
  printk("Goodbye,process_print");
}

module_init(print_init);
module_exit(print_exit);
MODULE_AUTHOR("welson");
MODULE_DESCRIPTION("Print Process Info.");
MODULE_LICENSE("GPL");
