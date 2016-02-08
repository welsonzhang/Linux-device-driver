#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>


//  �����豸�ļ���
#define DEVICE_NAME "wordcount"
static unsigned char mem[10000]; // �������豸�ļ�д�������
static int word_count = 0;
#define TRUE 255
#define FALSE 0

//  �ж�ָ���ַ��Ƿ�Ϊ�ո񣨰����ո�����Ʊ�����س����ͻ��з���
static unsigned char is_spacewhite(char c)
{
	if (c == 32 || c == 9 || c == 13 || c == 10)
		return TRUE;
	else
		return FALSE;
}

static int get_word_count(const char *buf)
{
	int n = 1;
	int i = 0;
	char c = ' ';

	char flag = 0; // �������ո�ָ��������0�����������1��������һ���ո�
	if (*buf == '\0')
		return 0;
	//  ��1���ַ��ǿո񣬴�0��ʼ����
	if (is_spacewhite(*buf) == TRUE)
		n--;

	//  ɨ���ַ����е�ÿһ���ַ�
	for (; (c = *(buf + i)) != '\0'; i++)
	{
		//  ֻ��һ���ո�ָ����ʵ����
		if (flag == 1 && is_spacewhite(c) == FALSE)
		{

			flag = 0;
		}
		//  �ɶ���ո�ָ����ʵ���������Զ���Ŀո�
		else if (flag == 1 && is_spacewhite(c) == TRUE)
		{

			continue;
		}
		//  ��ǰ�ַ�Ϊ�ո��ǵ�������1
		if (is_spacewhite(c) == TRUE)
		{
			n++;
			flag = 1;
		}
	}
	//  ����ַ�����һ�������ո��β������������������1��
	if (is_spacewhite(*(buf + i - 1)) == TRUE)
		n--;
	return n;
}

static ssize_t word_count_read(struct file *file, char __user *buf,
        size_t count, loff_t *ppos)
{
	unsigned char temp[4];

	temp[0] = word_count >> 24;
	temp[1] = word_count >> 16;   
	temp[2] = word_count >> 8;
	temp[3] = word_count;
	if (copy_to_user(buf, (void*) temp, 4))
	{
		return -EINVAL;
	}
	printk("read:word count:%d", (int) count);

	return count;
}

static ssize_t word_count_write(struct file *file, const char __user *buf,
        size_t count, loff_t *ppos)
{
	ssize_t written = count;

	if (copy_from_user(mem, buf, count))
	{
		return -EINVAL;
	}
	mem[count] = '\0';
	word_count = get_word_count(mem);
	printk("write:word count:%d\n", (int) word_count);

	return written;
}

//  �������豸�ļ��������¼���Ӧ�Ļص�����ָ��
static struct file_operations dev_fops =
{ .owner = THIS_MODULE, .read = word_count_read, .write = word_count_write };

//  �����豸�ļ�����Ϣ   
static struct miscdevice misc =
{ .minor = MISC_DYNAMIC_MINOR, .name = DEVICE_NAME, .fops = &dev_fops };


//  ��ʼ��Linux����
static int __init word_count_init(void)
{
	int ret; 

	//  �����豸�ļ�
	ret = misc_register(&misc);

	//  �����־��Ϣ
	printk("word_count_init_success\n");


	return ret;
}

// ж��Linux����
static void __exit word_count_exit(void)
{
	//  ɾ���豸�ļ�  
	misc_deregister(&misc);

	//  �����־��Ϣ
	printk("word_count_init_exit_success\n");
} 

//  ע���ʼ��Linux�����ĺ���
module_init( word_count_init);
//  ע��ж��Linux�����ĺ���
module_exit( word_count_exit);

MODULE_AUTHOR("lining");
MODULE_DESCRIPTION("statistics of word count.");
MODULE_ALIAS("word count module.");
MODULE_LICENSE("GPL");
