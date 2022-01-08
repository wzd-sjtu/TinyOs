#ifndef __THREAD_SYNC_H
#define __THREAD_SYNC_H
#include "list.h"
#include "stdint.h"
#include "thread.h"

struct semaphore {
    uint8_t value;
    struct list waiters;
};
struct lock {
    struct task_struct* holder;   // PBC进程控制块
    struct semaphore semaphore;  // 二元信号量
    uint32_t holder_repeat_nr; // repeat number
};

// function definition
void sema_init(struct semaphore* psema, uint8_t value);
void lock_init(struct lock* plock);
void sema_up(struct semaphore* psema);
void sema_down(struct semaphore* psema);
void lock_release(struct lock* plock);
void lock_acquire(struct lock* plock);

#endif