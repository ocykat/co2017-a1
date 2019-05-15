#include "progmem.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    struct proc_segs info;

    if (argc < 2) {
        printf("Please input the pid as a command line argument!\n");
    }
    else {
        pid_t pid = (pid_t) atoi(argv[1]);
        printf("pid = %d\n", (int) pid);

        if (progmem_syscall(pid, &info) == 0) {
            printf("progmem: successfully obtained memory layout!\n");
			printf("Student ID   : %lu\n", info.student_id);
			printf("Code segment : start = 0x%lx, end = 0x%lx\n", info.start_code, info.end_code);
			printf("Data segment : start = 0x%lx, end = 0x%lx\n", info.start_data, info.end_data);
			printf("Heap segment : start = 0x%lx, end = 0x%lx\n", info.start_heap, info.end_heap);
			printf("Stack segment: start = 0x%lx\n", info.start_stack);
        }
        else {
            printf("progmem: ERROR - could not obtain memory layout!\n");
        }
    }

    return 0;
}
