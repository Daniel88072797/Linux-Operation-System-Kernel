#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pagemap.h>      /* PAGE_CACHE_SIZE */
#include <linux/fs.h>           /* This is where libfs stuff is declared */
#include <asm/atomic.h>
#include <asm/uaccess.h>        /* copy_to_user */
 
 
MODULE_LICENSE("GPL");

#define MYFS_MAGIC 0x20210607
static int a = 0;
static int b = 0;
 
static struct inode *myfs_make_inode(struct super_block *sb, int mode)
{
        struct inode *ret = new_inode(sb);
 
        if (ret) {
                ret->i_mode = mode;
                ret->i_uid.val = 0;
                ret->i_gid.val = 0;
                ret->i_blocks = 0;
                ret->i_atime = current_kernel_time();
                ret->i_mtime = current_kernel_time();
                ret->i_ctime = current_kernel_time();
        }
        return ret;
}
 
static int myfs_open(struct inode *inode, struct file *filp)
{
        filp->private_data = inode->i_private;
        return 0;
}
 
#define TMPSIZE 20
 
static ssize_t myfs_read_file(struct file *filp, char *buf,
                size_t count, loff_t *offset)
{
        atomic_t *counter = (atomic_t *) filp->private_data;
        int v, len;
        char tmp[TMPSIZE];
	//printk("file name:%s",filp->f_path.dentry->d_name.name);

	if( strncmp(filp->f_path.dentry->d_name.name, "add", strlen("add")) == 0)
	{
		v = a + b;
	}
	else if(strncmp(filp->f_path.dentry->d_name.name, "sub", strlen("sub")) == 0){
		v = a - b;
	}
	else{
		v = atomic_read(counter);
	} 

	
        if (*offset > 0) v -= 1;  /* the value returned when offset was zero */
	
	

        len = snprintf(tmp, TMPSIZE, "%d\n", v);

        if (*offset > len)
                return 0;

        if (count > len - *offset)
                count = len - *offset;
 
        if (copy_to_user(buf, tmp + *offset, count))
                return -EFAULT;

        *offset += count;
        return count;
}
 
static ssize_t myfs_write_file(struct file *filp, const char *buf,
                size_t count, loff_t *offset)
{
        atomic_t *counter = (atomic_t *) filp->private_data;
        char tmp[TMPSIZE];
 
        if (*offset != 0)
                return -EINVAL;
 
        if (count >= TMPSIZE)
                return -EINVAL;
        memset(tmp, 0, TMPSIZE);
        if (copy_from_user(tmp, buf, count))
                return -EFAULT;
 
        atomic_set(counter, simple_strtol(tmp, NULL, 10));

	if(strncmp(filp->f_path.dentry->d_name.name, "a", strlen("a")) == 0){
		a = simple_strtol(tmp, NULL, 10);
	}
	else if(strncmp(filp->f_path.dentry->d_name.name, "b", strlen("b")) == 0){
		b = simple_strtol(tmp, NULL, 10);
	}	

	
        return count;
}
 
 
static struct file_operations myfs_file_ops = {
        .open   = myfs_open,
        .read   = myfs_read_file,
        .write  = myfs_write_file,
};
 
static struct dentry *myfs_create_file (struct super_block *sb,
                struct dentry *dir, const char *name,
                atomic_t *counter)
{
        struct dentry *dentry;
        struct inode *inode;
        struct qstr qname;
 
        qname.name = name;
        qname.len = strlen (name);
        qname.hash = full_name_hash(dir, name, qname.len);
 
        dentry = d_alloc(dir, &qname);
        if (! dentry)
                goto out;
        inode = myfs_make_inode(sb, S_IFREG | 0644);
        if (! inode)
                goto out_dput;
        inode->i_fop = &myfs_file_ops;
        inode->i_private = counter;
 
        d_add(dentry, inode);
        return dentry;
 
  out_dput:
        dput(dentry);
  out:
        return 0;
}
static struct dentry *myfs_create_dir (struct super_block *sb,
                struct dentry *parent, const char *name)
{
        struct dentry *dentry;
        struct inode *inode;
        struct qstr qname;
 
        qname.name = name;
        qname.len = strlen (name);
        qname.hash = full_name_hash(parent, name, qname.len);
        dentry = d_alloc(parent, &qname);
        if (! dentry)
                goto out;
 
        inode = myfs_make_inode(sb, S_IFDIR | 0644);
        if (! inode)
                goto out_dput;
        inode->i_op = &simple_dir_inode_operations;
        inode->i_fop = &simple_dir_operations;
 
        d_add(dentry, inode);
        return dentry;
 
  out_dput:
        dput(dentry);
  out:
        return 0;
}
 
static atomic_t a_counter, b_counter, add_counter, sub_counter;
 
static void myfs_create_files (struct super_block *sb, struct dentry *root)
{
        struct dentry *input;
	struct dentry *output;

 
        atomic_set(&a_counter, 0);
	atomic_set(&b_counter, 0);
        input = myfs_create_dir(sb, root, "input");
        if (input){
		myfs_create_file(sb, input, "a", &a_counter);
		myfs_create_file(sb, input, "b", &b_counter);
	}

        atomic_set(&add_counter, 0);
	atomic_set(&sub_counter, 0);
        output = myfs_create_dir(sb, root, "output");
        if (output){
		myfs_create_file(sb, output, "add", &add_counter);
		myfs_create_file(sb, output, "sub", &sub_counter);
	}
                


	
}
 
 
 
static struct super_operations myfs_s_ops = {
        .statfs         = simple_statfs,  //system provide
        .drop_inode     = generic_delete_inode, //system provide
};
 
static int myfs_fill_super (struct super_block *sb, void *data, int silent)
{
        struct inode *root;
        struct dentry *root_dentry;
 
        sb->s_blocksize = PAGE_SIZE;
        sb->s_blocksize_bits = PAGE_SHIFT;
        sb->s_magic = MYFS_MAGIC;
        sb->s_op = &myfs_s_ops;
 	
        root = myfs_make_inode (sb, S_IFDIR | 0755);
        if (! root)
                goto out;
        root->i_op = &simple_dir_inode_operations;
        root->i_fop = &simple_dir_operations;
 
        root_dentry = d_make_root(root);
        if (! root_dentry)
                goto out_iput;
        sb->s_root = root_dentry;
 
        myfs_create_files (sb, root_dentry);
        return 0;
 
  out_iput:
        iput(root);
  out:
        return -ENOMEM;
}
 
static struct dentry *myfs_get_super(struct file_system_type *fst,
                int flags, const char *devname, void *data)
{
        return mount_bdev(fst, flags, devname, data, myfs_fill_super); 
	//myfs_fill_super 初始化
}

static struct file_system_type myfs_type = {
        .owner          = THIS_MODULE,
        .name           = "myfs",
        .mount          = myfs_get_super,  //mount 時會call myfs_get_super
        .kill_sb        = kill_litter_super,
};
 
static int __init myfs_init(void)
{
        return register_filesystem(&myfs_type);
}
 
static void __exit myfs_exit(void)
{
        unregister_filesystem(&myfs_type);
}
 
module_init(myfs_init);
module_exit(myfs_exit);
