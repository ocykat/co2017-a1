#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>

#define SIZE 100
#define PROGMEM_SYSCALL_NUMBER 444

int main() {
    long sysvalue;
    unsigned long info[SIZE];
    sysvalue = syscall(PROGMEM_SYSCALL_NUMBER, 1, info);
    printf("sysvalue = %ld\n", sysvalue);
    printf("My Student ID: %lu\n", info[0]);
}
