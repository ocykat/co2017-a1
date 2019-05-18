#ifndef _PROC_MEM_H_
#define _PROC_MEM_H_

#include <unistd.h>
#include <sys/types.h>

// This struct should be identical to what we have defined in the file
// `linux-<version>/arch/x86/kernel/sys_progmem.c`
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

long procmem_syscall(pid_t pid, struct proc_segs* info);

#endif // _PROC_MEM_H_
