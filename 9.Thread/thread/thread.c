#include "thread.h"
#include "stdint.h"
#include "string.h"
#include "global.h"
#include "memory.h"
#include "interrupt.h"
#include "debug.h"

#define PG_SIZE 4096


static struct list_elem* thread_tag;
struct task_struct* main_thread; // 主线程
struct list thread_ready_list; //就绪队列
struct list thread_all_list; //所有线程队列


struct task_struct* running_thread() {
    uint32_t esp;
    asm ("mov %%esp, %0" : "=g" (esp));
    return (struct task_struct*)(esp&0xfffff000); // 前20位即为PCB指针地址
}
extern void switch_to(struct task_struct* cur, struct task_struct* next); // the switch of PCB

static void kernel_thread(thread_func* function, void* func_arg) {
    intr_enable(); // 需要开启时钟中断，防止操作系统控制权被剥夺
    function(func_arg); // just use it.
}
void thread_create(struct task_struct* pthread, thread_func function, void* func_arg) {
    pthread->self_kstack -= sizeof(struct thread_stack);
    struct thread_stack* kthread_stack = (struct thread_stack*)pthread->self_kstack;
    kthread_stack->eip = kernel_thread;
    kthread_stack->function = function;
    kthread_stack->func_arg = func_arg;
    kthread_stack->ebp = kthread_stack->ebx = \
    kthread_stack->edi = kthread_stack->esi = 0; // four register initial

}
void init_thread(struct task_struct* pthread, char* name, int prio) {
    memset(pthread, 0, sizeof(*pthread));
    strcpy(pthread->name, name);

    if(pthread == main_thread) {
        pthread->status = TASK_RUNNING;
    }
    else {
        pthread->status = TASK_READY;
    }

    pthread->status = TASK_RUNNING;
    pthread->priority = prio;

    pthread->self_kstack = (uint32_t*)((uint32_t)pthread + PG_SIZE); // a PCB's size is a page size
    pthread->stack_magic = 0x19870916;
    pthread->ticks = prio;
    pthread->pdgir = NULL;
    pthread->elapsed_ticks = 0;
}

struct task_struct* thread_start(char* name, int prio, \
            thread_func function, void* func_arg) {

    struct task_struct* thread = get_kernel_pages(1);

    init_thread(thread, name, prio);
    thread_create(thread, function, func_arg);

    // after create the thread, append it to the queue
    ASSERT(!elem_find(&thread_ready_list, &thread->general_tag));
    list_append(&thread_ready_list, &thread->general_tag);

    ASSERT(!elem_find(&thread_all_list, &thread->all_list_tag));
    list_append(&thread_all_list, &thread->all_list_tag);


    // 发现这句话居然不见了？
    // 用于创建线程的，是来临时测试借助ret引起线程运行的，可以注释掉
    /*
    asm volatile("movl %0, %%esp; pop %%ebp; pop %%ebx; pop %%edi; pop %%esi; \
        ret" : : "g" (thread->self_kstack) : "memory");
    */

    // 正式进入操作系统最复杂的部分，线程进程内存管理 锁机制，写的人头疼emm

    return thread;
}

static void make_main_thread(void) {
    // 刚刚进入内核时，总是主线程在跑
    main_thread = running_thread();
    init_thread(main_thread, "main", 31);

    ASSERT(!elem_find(&thread_all_list, &main_thread->all_list_tag));
    list_append(&thread_all_list, &main_thread->all_list_tag);
}

void thread_init(void) {
    put_str("\nthread_init start!\n");
    list_init(&thread_ready_list);
    list_init(&thread_all_list);
    make_main_thread(); // main thread initializing

    put_str("\nthread_init end!\n");
}