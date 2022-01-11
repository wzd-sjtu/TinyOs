#include "print.h"
#include "init.h"
#include "thread.h"
#include "interrupt.h"
#include "console.h"
#include "process.h"
#include "syscall.h"
#include "syscall-init.h"

void k_thread_a(void*);
void k_thread_b(void*);
void u_prog_a(void);
void u_prog_b(void);
int prog_a_pid = 0, prog_b_pid = 0;

int main(void) {
   put_str("I am kernel\n");
   init_all();

   // process_execute(u_prog_a, "user_prog_a");
   // process_execute(u_prog_b, "user_prog_b");

    // directly GG!

   console_put_int(sys_getpid());

   thread_start("k_thread_a", 31, k_thread_a, "argA ");
   thread_start("k_thread_b", 31, k_thread_b, "argB ");

   process_execute(u_prog_a, "user_prog_a");
   // process_execute(u_prog_b, "user_prog_b");


   intr_enable();

   while(1);
   return 0;
}

/* 在线程中运行的函数 */
void k_thread_a(void* arg) {     
   char* para = arg;
   //while(1) {
      console_put_str(" thread pid:0x");
      console_put_int(sys_getpid());
   //}
   while(1);
}

/* 在线程中运行的函数 */
void k_thread_b(void* arg) {     
   char* para = arg;
   //while(1) {
      console_put_str(" thread pid:0x");
      console_put_int(sys_getpid());
   //}
   while(1);
}

/* 测试用户进程 */
void u_prog_a(void) {
   //while(1) {
      // console_put_str(" pid:0x");
      // in fact, it is because getpid function's cover is not enough

      prog_a_pid = getpid();
      printf("I am happy\n");
   //}
   while(1);
}

/* 测试用户进程 */
void u_prog_b(void) {
   //while(1) {
      // console_put_str(" pid:0x");
      prog_b_pid = getpid();
      printf("i am happy!");
   //}
   while(1);
}
