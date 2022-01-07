#include "print.h"
#include "init.h"
#include "debug.h"
#include "memory.h"
#include "thread.h"

void k_thread_a(void*);
void k_thread_b(void*);
int main(void) {

    put_str("wzd is handsome!\nwzd is also beautiful!\n");
    init_all();

    // ASSERT(1==2);
    // asm volatile("sti"); // temporaryly open the interrupt

    /*
    put_str("\nget kernel pages!\n");
    void* addr = get_kernel_pages(3);
    put_int((uint32_t)addr);
    put_str("\nget kernel pages done!\n");
    */

    // by now, we eventually finish the PCB structure.

    // test the thread
    put_str("\nbegin the thread!\n");
    thread_start("k_thread_a", 31, k_thread_a, "argA ");
    thread_start("k_thread_b", 8, k_thread_b, "argB ");

    put_str("\nend the thread!\n");

    intr_enable();
    while(1) {
        intr_disable();
        put_str("Main! ");
        intr_enable();
    }
    while(1);
    return 0;
}

void k_thread_a(void* arg) {
    char* para = arg;
    while(1) {
        intr_disable();
        put_str(arg);
        intr_enable();
    }
}
void k_thread_b(void* arg) {
    char* para = arg;
    while(1) {
        intr_disable();
        put_str(arg);
        intr_enable();
    }
}