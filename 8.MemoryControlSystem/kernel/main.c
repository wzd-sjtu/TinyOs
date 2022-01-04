#include "print.h"
#include "init.h"
#include "debug.h"
#include "memory.h"

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
    while(1);
    return 0;
}