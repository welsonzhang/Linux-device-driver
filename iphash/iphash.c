#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/rcupdate.h>

MODULE_LICENSE("Dual BSD/GPL");

unsigned num_buckets = 65535;

struct conn_bucket{
	struct hlist_head list;
	spinlock_t lock;
	u16 depth;
};

struct ipinfo{
	struct hlist_node hashnode;
	u32 ip_addr;
};

struct conn_bucket *g_conn_bucket;

int init_hash()
{
    int i;
    if (num_buckets <= 0) {
        printk("tcpa:num_buckets need more than 1\n");
        return -EINVAL;
    }
    
    g_conn_bucket = (struct conn_bucket* )vmalloc(sizeof(struct conn_bucket)*num_buckets);
    if (!g_conn_bucket) {
        return -ENOMEM;
    }
    memset(g_conn_bucket, 0, sizeof(struct conn_bucket)*num_buckets);

    for(i = 0 ; i < num_buckets; i++) {
        struct conn_bucket *bucket = &g_conn_bucket[i];
	INIT_HLIST_HEAD(&bucket->list);
	spin_lock_init(&bucket->lock);
        bucket->depth = 0;
    }
    return 0;
}

void show_hash(void)
{
#if 1
	int i;
	struct hlist_head *head;
	struct hlist_node *pos, *n;
	struct ipinfo *info;

	for(i = 0; i < num_buckets; i++) {
		struct conn_bucket *bucket = &g_conn_bucket[i];
		head = &bucket->list;
		pos = NULL;
        	n = NULL;
		spin_lock_bh(&bucket->lock);

		if(!hlist_empty(head)) {
        		hlist_for_each_safe(pos, n, head) {
                		info = list_entry(pos, struct ipinfo, hashnode);
                		printk("%x\n", info->ip_addr);
        		}
		}
		
		spin_unlock_bh(&bucket->lock);

	}
#endif
}

void clear_hash()
{
	int i;
	struct hlist_head *head;
	struct hlist_node *pos, *n;
	struct ipinfo *info;
	
	for(i = 0; i < num_buckets; i++)
	{
		struct conn_bucket *bucket = &g_conn_bucket[i];
		spin_lock_bh(&bucket->lock);
		head = &bucket->list;
		pos = NULL;
		n = NULL;
		hlist_for_each_entry_safe(info, n, head, hashnode) {
			;
		}	
		spin_unlock_bh(&bucket->lock);
		
	}
	vfree(g_conn_bucket);
}

u16 ipkey(u32 ip_addr)
{
	u16 key = (u16)ip_addr;
	key = key % num_buckets;
	return key; 
}

int hash_add(u32 ip_addr)
{	
	u16 key;
	unsigned exist = 0;
	struct ipinfo *info_ptr;
	struct hlist_head *head;
	struct hlist_node *pos, *n;

	struct ipinfo *info = kmalloc(sizeof(struct ipinfo), GFP_ATOMIC);
	if(!info) {
		return info;
	}
	info->ip_addr = ip_addr;
	key = ipkey(ip_addr);
	struct conn_bucket *bucket = &g_conn_bucket[key];

	spin_lock_bh(&bucket->lock);
       	pos = NULL;
        n = NULL;
	head = &bucket->list;
	if(!hlist_empty(head)) {
        	hlist_for_each_entry_safe(info, n, head, hashnode) {
       			info_ptr = list_entry(pos, struct ipinfo, hashnode);
			if(info_ptr->ip_addr == ip_addr) {
				exist = 1;
				break;
			} 
        	}
	}
	if(exist == 0) {
		hlist_add_head(&info->hashnode, head);
		bucket->depth++;
		if(bucket->depth > 100) {
			printk("bucket depth is too long!\n");
		}
	}
	spin_unlock_bh(&bucket->lock);

	return 0;
}

static int hello_init(void)
{
	int ret;
	ret = init_hash();
	u32 addr = 0x0A0000FF;
	ret = hash_add(addr);
	printk("Hello, world\n");
}

static void hello_exit(void)
{
	show_hash();

	struct ipinfo *info;
	struct hlist_node *pos;
	struct hlist_node *n;
	struct hlist_head *head;
	u32 addr = 0x0A0000FF;
	u16 key = ipkey(addr);
	struct conn_bucket *bucket = &g_conn_bucket[key];
	
	head = &bucket->list;
        pos = NULL;
        n = NULL;

        read_lock_bh(&bucket->lock);
	if(!hlist_empty(head)) {
        	hlist_for_each_safe(pos, n, head) {
        		info = list_entry(pos, struct ipinfo, hashnode);
                	printk("%x\n", info->ip_addr);
        	}
	}
        read_unlock_bh(&bucket->lock);
	clear_hash();	
	printk("Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
