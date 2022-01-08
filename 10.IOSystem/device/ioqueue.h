#ifndef __DEVICE_IOQUEUE_H
#define __DEVICE_IOQUEUE_H
#include "stdint.h"
#include "thread.h"
#include "sync.h"

#define bufsize 64

struct ioqueue {
    struct lock lock;
    struct task_struct* producer;

    struct task_struct* consumer;
    char buf[bufsize];

    int32_t head;
    int32_t tail; // this is for tail
};

#endif