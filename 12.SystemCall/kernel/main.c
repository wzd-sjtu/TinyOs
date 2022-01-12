#include "print.h"
#include "init.h"
#include "thread.h"
#include "interrupt.h"
#include "console.h"
#include "process.h"
#include "syscall.h"
#include "syscall-init.h"
#include "stdio.h"

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
   // somehow there is mistake, why did it happen?
   console_put_int(sys_getpid());

   thread_start("k_thread_a", 31, k_thread_a, "argA ");
   thread_start("k_thread_b", 31, k_thread_b, "argB ");

   intr_enable(); // begin the timer at the beginning

   process_execute(u_prog_a, "user_prog_a");
   process_execute(u_prog_b, "user_prog_b");

   while(1);
   return 0;
}

/* 在线程中运行的函数 */
void k_thread_a(void* arg) {     
   char* para = arg;
   //while(1) {
      console_put_str(" thread pid:0x\n");
      console_put_int(sys_getpid());
   //}
   while(1);
}

/* 在线程中运行的函数 */
void k_thread_b(void* arg) {     
   char* para = arg;
   //while(1) {
      console_put_str(" thread pid:0x\n");
      console_put_int(sys_getpid());
   //}
   while(1);
}

/* 测试用户进程 */
void u_prog_a(void) {
   printf("I am happy!:0x%x\n", getpid());
   while(1);
}

/* 测试用户进程 */
void u_prog_b(void) {
   //while(1) {
      // console_put_str(" pid:0x");
      printf("i am happy!\n");
      prog_b_pid = getpid();
   //}
   while(1);
}
