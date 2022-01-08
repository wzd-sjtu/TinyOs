#include "ioqueue.h"
#include "interrupt.h"
#include "global.h"
#include "debug.h"

void ioqueue_init(struct ioqueue* ioq) {
    // init the queue
    lock_init(&ioq->lock);
    ioq->producer = ioq->consumer = NULL;
    ioq->head = ioq->tail = 0;
}

static int32_t next_pos(int32_t pos) {
    return (pos+1)%bufsize;
}
// there is no bool in c language
int ioq_full(struct ioqueue* ioq) {
    ASSERT(intr_get_status()==INTR_OFF); // off the clock
    return next_pos(ioq->head) == ioq->tail;
}
static int ioq_empty(struct ioqueue* ioq) {
    return ioq->head==ioq->tail;
}
static void ioq_wait(struct ioqueue** waiter) {
    *waiter = running_thread();
    thread_block(TASK_BLOCKED);
}
static void wakeup(struct ioqueue** waiter) {
    thread_unblock(*waiter);
    *waiter = NULL; // only blocked. will be produced?
}
char ioq_getchar(struct ioqueue* ioq) {
    while(ioq_empty(ioq)) {
        lock_acquire(&ioq->lock);
        ioq_wait(&ioq->consumer); // block in consumer
        lock_release(&ioq->lock);
    }
    char byte = ioq->buf[ioq->tail];
    ioq->tail = next_pos(ioq->tail);

    if(ioq->producer != NULL) {
        wakeup(&ioq->producer);
    }

    return byte;
}

void ioq_putchar(struct ioqueue* ioq, char byte) {
    while(ioq_full(ioq)) {
        lock_acquire(&ioq->lock);
        ioq_wait(&ioq->producer);
        lock_release(&ioq->lock);
    }
    ioq->buf[ioq->head] = byte;
    ioq->head = next_pos(ioq->head);

    if(ioq->consumer != NULL) {
        wakeup(&ioq->consumer); // circle ioqueue
    }
}

// these are all things for ioq!