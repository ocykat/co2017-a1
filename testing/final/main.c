#include <procmem.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
int main() {
    pid_t mypid = getpid();
    printf("PID: %d\n", mypid);
    struct proc_segs info;

    if (procmem_syscall(mypid, &info) == 0) {
        printf("Student ID: %lu \n", info.student_id);
        printf("Code segment: %lx-%lx\n", info.start_code, info.end_code);
        printf("Data segment: %lx-%lx\n", info.start_data, info.end_data);
        printf("Heap segment: %lx-%lx\n", info.start_heap, info.end_heap);
        printf("Start stack: %lx\n", info.start_stack);
    } else {
        printf("Cannot get information from the process %d\n", mypid);
    }
    // If necessary, uncomment the following line to make this program run
    // long enough so that we could check its maps file
    sleep(100);
}
