/* the version of kernel source:4.14.305 */


#include <linux/init.h>
#include <linux/module.h>
  
#include <linux/sched/signal.h>
//#include <linux/sched.h>
  
MODULE_LICENSE("Dual BSD/GPL");
  
struct task_struct *task;  //Structure defined in sched.h for tasks/processes
struct task_struct *task_child; // Structure needed to iterate through task children
struct list_head *list; //Structure needed to iterate through the list in each task->children struct

static int process_init(void)
{
  printk(KERN_INFO "%s","LOADING MODULE\n");

  for_each_process( task ){ 

    printk(KERN_INFO "PID: %d, UID: %u, PROCESS: %s \n"
    ,task->pid, task->real_cred->uid, task->comm); 

  }
        return 0;
}
  
static void process_exit(void)
{
        printk(KERN_INFO "%s","REMOVING MODULE\n");
}
  
module_init(process_init);
module_exit(process_exit);
