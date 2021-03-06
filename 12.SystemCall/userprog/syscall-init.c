#include "thread.h"
#include "syscall.h"
#include "stdint.h"
#include "console.h"
#include "string.h"
#include "print.h"
#include "memory.h"

#define syscall_nr 32
typedef void* syscall;

syscall syscall_table[syscall_nr]; // vector table is here, function pointer vector

uint32_t sys_getpid(void) {
    return running_thread()->pid;
}

// someplace writes wronly! damm it!
uint32_t sys_write(char* str) {
    int m = strlen(str);
    console_put_int(m);
    str[m] = '\0';
    console_put_str(str);
    return strlen(str);
}

void syscall_init(void) {
    // in the kernel mode, no need to satisfy the situation
    put_str("syscall_table init begin!\n");
    syscall_table[SYS_GETPID] = sys_getpid; // function pointer here, which is in fact a normal pointer
    syscall_table[SYS_WRITE] = sys_write; // function of printf
    syscall_table[SYS_MALLOC] = sys_malloc;
    syscall_table[SYS_FREE] = sys_free;
    put_str("syscall_table init end!\n");
}

