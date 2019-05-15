#include "procmem.h"
#include <linux/kernel.h>
#include <sys/syscall.h>

long progmem_syscall(pid_t pid, struct proc_segs* info) {
    long sysvalue;
    unsigned long info[SIZE];

    sysvalue = syscall(PROGMEM_SYSCALL_NUMBER, pid, info);

    return sysvalue;
}
