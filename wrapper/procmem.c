#include "procmem.h"
#include <linux/kernel.h>
#include <sys/syscall.h>

#define PROCMEM_SYSCALL_NUMBER 444
#define SIZE                   100

long procmem_syscall(pid_t pid, struct proc_segs* info) {
    long sysvalue = syscall(PROCMEM_SYSCALL_NUMBER, pid, info);
    return sysvalue;
}
