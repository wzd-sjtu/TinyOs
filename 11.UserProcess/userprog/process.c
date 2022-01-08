#include "thread.h"
#include "global.h"
#include "stdint.h"
#include "memory.h"
#include "userprog.h"
#include "debug.h"
#include "string.h"
#include "process.h"
#include "interrupt.h"
#include "console.h"
#include "sync.h"

extern struct list thread_ready_list;	    // 就绪队列
extern struct list thread_all_list;	    // 所有任务队列

extern void intr_exit(void); // extern function for basic need

void start_process(void* filename_) {
    void* function = filename_;

    struct task_struct* cur = running_thread();
    cur->self_kstack += sizeof(struct thread_stack); // kernel stack?
    struct intr_stack* proc_stack = (struct intr_stack*)cur->self_kstack;

    // initialization of the common register
    proc_stack->edi = proc_stack->esi = \
    proc_stack->ebp = proc_stack->esp_dummy = 0;
    proc_stack->ebx = proc_stack->edi = \
    proc_stack->ecx = proc_stack->eax = 0;

    proc_stack->gs = 0;
    proc_stack->ds = proc_stack->es = proc_stack->fs = SELECTOR_U_DATA;

    proc_stack->eip = function;
    proc_stack->cs = SELECTOR_U_CODE;
    proc_stack->eflags = (EFLAGS_IOPL_0 | EFLAGS_MBS | EFLAGS_IF_1);
    proc_stack->esp = (void*)((uint32_t)get_a_page(PF_USER, \
        USER_STACK3_VADDR)+PG_SIZE);
    proc_stack->ss = SELECTOR_U_DATA; // selector for gdt

    // inline assemble add to the esp
    asm volatile ("movl %0, %%esp; jmp intr_exit" \
    : : "g" (proc_stack) : "memory");
}

void page_dir_activate(struct task_struct* p_thread) {
    uint32_t pagedir_phy_addr = 0x100000;
    if(p_thread->pgdir != NULL) {
        pagedir_phy_addr = addr_v2p((uint32_t)p_thread->pgdir);
    }

    asm volatile ("movl %0, %%cr3" : : "r" (pagedir_phy_addr) : "memory");

}

void process_activate(struct task_struct* p_thread) {
    ASSERT(p_thread!=NULL);

    page_dir_activate(p_thread);

    if(p_thread->pgdir) {
        update_tss_esp(p_thread); // uncomplete function here
    }
}


/* 创建页目录表,将当前页表的表示内核空间的pde复制,
 * 成功则返回页目录的虚拟地址,否则返回-1 */
uint32_t* create_page_dir(void) {

   /* 用户进程的页表不能让用户直接访问到,所以在内核空间来申请 */
   uint32_t* page_dir_vaddr = get_kernel_pages(1);
   if (page_dir_vaddr == NULL) {
      console_put_str("create_page_dir: get_kernel_page failed!");
      return NULL;
   }

/************************** 1  先复制页表  *************************************/
   /*  page_dir_vaddr + 0x300*4 是内核页目录的第768项 */
   memcpy((uint32_t*)((uint32_t)page_dir_vaddr + 0x300*4), (uint32_t*)(0xfffff000+0x300*4), 1024);
/*****************************************************************************/

/************************** 2  更新页目录地址 **********************************/
   uint32_t new_page_dir_phy_addr = addr_v2p((uint32_t)page_dir_vaddr);
   /* 页目录地址是存入在页目录的最后一项,更新页目录地址为新页目录的物理地址 */
   page_dir_vaddr[1023] = new_page_dir_phy_addr | PG_US_U | PG_RW_W | PG_P_1;
/*****************************************************************************/
   return page_dir_vaddr;
}

/* 创建用户进程虚拟地址位图 */
void create_user_vaddr_bitmap(struct task_struct* user_prog) {
   user_prog->userprog_vaddr.vaddr_start = USER_VADDR_START;
   uint32_t bitmap_pg_cnt = DIV_ROUND_UP((0xc0000000 - USER_VADDR_START) / PG_SIZE / 8 , PG_SIZE);
   user_prog->userprog_vaddr.vaddr_bitmap.bits = get_kernel_pages(bitmap_pg_cnt);
   user_prog->userprog_vaddr.vaddr_bitmap.btmp_bytes_len = (0xc0000000 - USER_VADDR_START) / PG_SIZE / 8;
   bitmap_init(&user_prog->userprog_vaddr.vaddr_bitmap);
}

/* 创建用户进程 */
void process_execute(void* filename, char* name) { 
   /* pcb内核的数据结构,由内核来维护进程信息,因此要在内核内存池中申请 */
   struct task_struct* thread = get_kernel_pages(1);
   init_thread(thread, name, default_prio); 
   create_user_vaddr_bitmap(thread);
   thread_create(thread, start_process, filename);
   thread->pgdir = create_page_dir();
   
   enum intr_status old_status = intr_disable();
   ASSERT(!elem_find(&thread_ready_list, &thread->general_tag));
   list_append(&thread_ready_list, &thread->general_tag);

   ASSERT(!elem_find(&thread_all_list, &thread->all_list_tag));
   list_append(&thread_all_list, &thread->all_list_tag);
   intr_set_status(old_status);
}
