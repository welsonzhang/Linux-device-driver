#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/slab.h>

static struct kmem_cache *cachep;

struct data
{
  char *name;
  int value;
}*datap;

static int __init slab_cache_init()
{
  cachep=kmem_cache_create("slap_test",sizeof(struct data),0,SLAB_HWCACHE_ALIGN|SLAB_PANIC,NULL);
  datap=kmem_cache_alloc(cachep,GFP_KERNEL);
  datap->name="slap data";
  datap->value=1234;
  printk(KERN_ALERT "slab cache init.\n");

  return 0;
}

static void __exit slab_cache_exit()
{
  if(datap)
  {
   printk(KERN_ALERT "data.name=%s\n",datap->name);
   printk(KERN_ALERT "data.value=%d\n",datap->value);
   kmem_cache_free(cachep,datap);
   kmem_cache_destroy(cachep);
  }
  printk(KERN_ALERT "slab cache exit.\n");
}

MODULE_LICENSE("GPL");
module_init(slab_cache_init);
module_exit(slab_cache_exit);
