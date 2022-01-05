#ifndef __THREAD_THREAD_H
#define __THREAD_THREAD_H

#include "stdint.h"

typedef void thread_func(void*);

enum task_status {
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_WAITING,
    TASK_HANGING,
    TASK_DIED
};

// 中断栈，当中断发生时，保存内核环境
struct intr_stack {
    uint32_t vec_no; // vector 中断向量号
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy; // esp is no use
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    // cpu 从低特权级到高特权级时压栈的内容
    /*
    section .text
    global intr_exit
    intr_exit:
    add esp, 4 ;what is this thing for? for push %1
    popad
    pop gs
    pop fs
    pop es
    pop ds
    add esp, 4 ; jump error code
    iretd
    */

    // 结构有一定的相似之处
    uint32_t err_code;
    void (*eip)(void);
    uint32_t cs;
    uint32_t eflags;
    void* esp;
    uint32_t ss; // cs+ss 程序计数器指针
};

// 线程栈
struct thread_stack {
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edi;
    uint32_t esi;
    void (*eip)(thread_func* func, void* func_arg);

    void (*unused_retaddr);
    thread_func* function;
    void* func_arg;

};

// 进程控制块PCB
struct task_struct {
    uint32_t* self_kstack;
    enum task_status status;
    uint8_t priority;
    char name[6];
    uint32_t stack_magic; // 用于检测栈溢出
};

struct task_struct* thread_start(char* name, int prio, \
            thread_func function, void* func_arg);

#endif