#include "thread.h"
#include "syscall.h"
#include "stdint.h"
#include "console.h"
#include "string.h"
#include "print.h"

#define syscall_nr 32
typedef void* syscall;

syscall syscall_table[syscall_nr]; // vector table is here, function pointer vector

uint32_t sys_getpid(void) {
    return running_thread()->pid;
}

void syscall_init(void) {
    put_str("syscall_table init begin!\n");
    syscall_table[SYS_GETPID] = sys_getpid; // function pointer here, which is in fact a normal pointer
    put_str("syscall_table init end!\n");
}

