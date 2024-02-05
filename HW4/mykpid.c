#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/signal.h>
#include <linux/pid.h>

MODULE_LICENSE("Dual BSD/GPL");
static struct proc_dir_entry *proc_entry;
static ssize_t mykpid_write(struct file *file, const char __user *buff, size_t len,  loff_t *pos);

static char pid_msg[10];
static char int_msg[4];
static char int_msg2[] = "INT";
static bool i = 0;
static struct pid *pid_struct;


static struct file_operations my_ops={
.write=mykpid_write,
.owner=THIS_MODULE,
};
extern int kill_pid(struct pid *pid, int sig, int priv); 

static ssize_t mykpid_write(struct file *file, const char __user *buff, size_t len,  loff_t *pos)
{
	
    unsigned long len2 = len;
	unsigned int kpid;
	
	if(i == 0){

		
		if (len2>=sizeof(pid_msg))
			len2=sizeof(pid_msg)-1;
		if (raw_copy_from_user(pid_msg, buff, len2))
		    return -EFAULT;	
		pid_msg[len2] = '\0';

		kstrtouint(pid_msg, 0, &kpid);
		pid_struct = find_vpid(kpid);
					
		printk(KERN_ERR "get pid : %d\n", kpid);
		i = 1;
	}
	else{

		if (len2>=sizeof(int_msg))
			len2=sizeof(int_msg)-1;
		if (raw_copy_from_user(int_msg, buff, len2))
		    return -EFAULT;	
		int_msg[len2] = '\0';
	
		if(strcmp(int_msg, int_msg2) == 0){

			kill_pid(pid_struct, SIGINT, 1);

		}
		printk(KERN_ERR "kill process\n");
		i = 0;
	}
	
	
	
    return len;
}
 
static int __init mykpid_init(void)
{
        proc_entry = proc_create("mykpid", 0666, NULL,&my_ops);
        if (!proc_entry) {
                printk(KERN_ERR "Can't create /proc/mykpid\n");
                return -1;
        }
        return 0;
}
 
static void __exit mykpid_exit(void)
{
        remove_proc_entry("mykpid", NULL);
}
 
module_init(mykpid_init);
module_exit(mykpid_exit);
