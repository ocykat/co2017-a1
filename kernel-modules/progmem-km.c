#include <linux/module.h>       // for all kernel modules
#include <linux/kernel.h>       // for KERN_INFO
#include <linux/init.h>         // for __init and __exit macros
#include <linux/mm.h>           // for struct mm_struct
#include <linux/sched.h>        // for struct task_struct
#include <linux/sched/signal.h> // for for_each_process macro

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nhat Nguyen");

static int pid;

struct proc_segs {
    unsigned long student_id;
    unsigned long start_code;
    unsigned long end_code;
    unsigned long start_data;
    unsigned long end_data;
    unsigned long start_heap;
    unsigned long end_heap;
    unsigned long start_stack;
};

static int __init procmem_init(void)
{
    struct task_struct* task;

    printk(KERN_INFO "Starting kernel module \"progmem-km\"!\n");

    for_each_process(task) {
        printk("pid : %d - name: %s\n", task->pid, task->comm);

        if(task->pid == pid) {
            if(task->mm != NULL) {
                printk("=> FOUND!!\n");
                printk("Code Segment: start = 0x%lx, end = 0x%lx\n",
                        task->mm->start_code, task->mm->end_code);
                printk("Data Segment: start = 0x%lx, end = 0x%lx\n",
                        task->mm->start_data, task->mm->end_data);
                printk("Heap Segment: start = 0x%lx, end = 0x%lx\n",
                        task->mm->start_brk, task->mm->brk);
                printk("Stack Segment: start = 0x%lx\n", task->mm->start_stack);
                printk("---------------------------------------\n");
                return 0;
            }
        }
    }

    return 0;
}

static void __exit procmem_cleanup(void)
{
    printk(KERN_INFO "Cleaning up module.\n");
}

module_init(procmem_init);
module_exit(procmem_cleanup);
module_param(pid, int , 0);
