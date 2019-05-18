/**
 * This file should be located in: `linux-<version>/arch/x86/kernel/`
 */

#include <linux/linkage.h>
#include <linux/sched.h>        // `struct task_struct`
#include <linux/mm.h>           // `struct mm_struct`
#include <linux/sched/signal.h> // `for_each_process` macro
#include <linux/uaccess.h>      // `copy_to_user` function

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

/**
 * sys_procmem() - get memory layout of process
 * @pid:  pid of the process
 * @info: memory layout information
 *
 * Return: 0 if process with given pid found, -1 otherwise.
 */
asmlinkage long sys_procmem(int pid, struct proc_segs* info) {
    struct task_struct* task;
    struct proc_segs tmp;
    unsigned long uncopied_bytes;

    // We cannot directly assign values to `info`.
    // Instead, we use the function `copy_to_user()` to copy the data
    // from kernel-space to user-space

    printk("sys_procmem: starting...\n");

    for_each_process(task) {
        if(task->pid == pid) {
            if(task->mm != NULL) {
                tmp.student_id  = 1752039;
                tmp.start_code  = task->mm->start_code;
                tmp.end_code    = task->mm->end_code;
                tmp.start_data  = task->mm->start_data;
                tmp.end_data    = task->mm->end_data;
                tmp.start_heap  = task->mm->start_brk;
                tmp.end_heap    = task->mm->brk;
                tmp.start_stack = task->mm->start_stack;

                uncopied_bytes = copy_to_user(info, &tmp, sizeof(tmp));

                if (uncopied_bytes == 0) {
                    printk("sys_procmem: copied data from kernel successfully\n");
                }
                else {
                    printk("sys_procmem: failed to copy data from kernel\n");
                }

                return 0;
            }
        }
    }

    printk("sys_procmem: cannot find the process with the pid %d\n", pid);

    return -1;
}
