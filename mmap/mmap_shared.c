#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>

#define DEVICE_NAME "mmap_shared"
#define BUFFER_SIZE 4096

static char *buffer;

static void demo_vma_open(struct vm_area_struct *vma)
{
  printk(KERN_INFO "VMA open.\n");
}

static void demo_vma_close(struct vm_area_struct *vma)
{
  printk(KERN_INFO "VMA close.\n");
}

static struct vm_operations_struct remap_vm_ops=
{
 .open=demo_vma_open,
 .close=demo_vma_close
};

static int demo_mmap(struct file *filp,struct vm_area_struct *vma)
{
  unsigned long physics=virt_to_phys((void*)(unsigned long)buffer);
  unsigned long mypfn=physics>>PAGE_SHIFT;
  unsigned long vmsize=vma->vm_end-vma->vm_start;

  printk(KERN_INFO "demo mmap called.\n");
  if(vmsize > BUFFER_SIZE)
  {
   return -EINVAL;
  }

  vma->vm_ops=&remap_vm_ops;
  vma->vm_flags|=(VM_IO | VM_LOCKED | (VM_DONTEXPAND | VM_DONTDUMP));
  demo_vma_open(vma);
  
  if(remap_pfn_range(vma,vma->vm_start,mypfn,vmsize,vma->vm_page_prot))
  {
    return -EAGAIN;
  }

  return 0;
}

static struct file_operations dev_fops=
{
 .owner=THIS_MODULE,
 .mmap=demo_mmap
};

static struct miscdevice misc=
{
 .minor=MISC_DYNAMIC_MINOR,
 .name=DEVICE_NAME,
 .fops=&dev_fops
};

static int __init mmap_shared_init()
{
  int ret;
  struct page *page;
  ret=misc_register(&misc);
  buffer=kmalloc(BUFFER_SIZE,GFP_KERNEL);

  for(page=virt_to_page(buffer);page<virt_to_page(buffer+BUFFER_SIZE);page++)
  {
   SetPageReserved(page);
  }

  memset(buffer,0,BUFFER_SIZE);
  strcpy(buffer,"mmap_shared_success.\n");
  printk(KERN_INFO "mmap demo init.\n");
  
  return ret;
}

static void __exit mmap_shared_exit()
{
  struct page *page;
  misc_deregister(&misc);
 
  for(page=virt_to_page(buffer);page<virt_to_page(buffer+BUFFER_SIZE);page++)
  {
   ClearPageReserved(page);
  }
  printk(KERN_INFO "mmap exit.\n");
}

MODULE_LICENSE("GPL");
module_init(mmap_shared_init);
module_exit(mmap_shared_exit);
