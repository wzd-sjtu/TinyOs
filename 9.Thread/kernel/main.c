#include "print.h"
#include "init.h"
#include "debug.h"
#include "memory.h"
// #include "thread.h"

void k_thread_a(void*);

int main(void) {
    
    /*
    put_char('w');
    put_char('z');
    put_char('d');
    put_char(' ');
    put_char('h');
    put_char('a');
    put_char('p');
    put_char('p');
    put_char('y');
    put_char('!');
    put_char('\n');
    put_char('w');
    */

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
    thread_start("k_thread_a", 31, k_thread_a, "argA ");
    thread_start("k_thread_b", 8, k_thread_a, "argB ");

    intr_enable();
    while(1) {
        put_str("Main! ");
    }
    while(1);
    return 0;
}

void k_thread_a(void* arg) {
    char* para = arg;
    while(1) {
        put_str(arg);
    }
}