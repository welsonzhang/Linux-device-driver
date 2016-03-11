#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>

static int __init time_init()
{
  struct timeval tv;
  struct timespec ts;
  
  ts.tv_nsec=0;
  ts.tv_sec=(unsigned long)mktime(2009,9,28,17,55,30);
  
  do_gettimeofday(&tv);
  printk("sec:%ld,nsec:%ld\n",tv.tv_sec,tv.tv_usec);

  printk("setting success:%d\n",do_settimeofday(&ts));
  printk("time_init success.\n");

  return 0;
}

static void __exit time_exit()
{
  printk("time exit success.\n");
}

MODULE_LICENSE("GPL");
module_init(time_init);
module_exit(time_exit);
