#ifndef __THREAD_SWITCH_H
#define __THREAD_SWITCH_H
#include "thread.h"

void switch_to(struct task_struct* cur, struct task_struct* next);

#endif

// directly happens a extremely mistake, unfortunate!
