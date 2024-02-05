#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
 
MODULE_LICENSE("Dual BSD/GPL");
 
/*
 * These lines, although not shown in the book,
 * are needed to make hello.c run properly even when
 * your kernel has version support enabled
 */
 
 
/*
 * A couple of parameters that can be passed in: how many times we say
 * hello, and to whom.
 */
static int a = 0;
static int b = 0;
static char *c = "world";

module_param(a, int, S_IRUGO);
module_param(b, int, S_IRUGO);
module_param(c, charp, S_IRUGO);
 
static int hello_init(void)
{
        int i;
        for (i = 0; i < a * b; i++)
                printk(KERN_ALERT "(%d)%s\n", i, c);
        return 0;
}
 
static void hello_exit(void)
{
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
 
module_init(hello_init);
module_exit(hello_exit);
