#include "print.h"
#include "init.h"
#include "interrupt.h"
#include "../device/timer.h"
#include "memory.h"

void init_all() {
    put_str("init_all!\n");
    idt_init();
    timer_init();
    mem_init();  // the usage of memory init faied! dam it!
}