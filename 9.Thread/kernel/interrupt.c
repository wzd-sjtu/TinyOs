#include "interrupt.h"
#include "stdint.h"
#include "global.h"
#include "io.h"
#include "print.h"
#include "thread.h"
#include "debug.h"
#include "list.h"
#include "switch.h"
// add an extra element of almost everything, right.
#define IDT_DESC_CNT 0x21

#define PIC_M_CTRL 0x20
#define PIC_M_DATA 0x21
#define PIC_S_CTRL 0xa0
#define PIC_S_DATA 0xa1
// init the pic 8259A

#define EFLAGS_IF 0x00000200 // eflags if=1 forbid interrupt
#define GET_EFLAGS(EFLAG_VAR) asm volatile("pushfl; popl %0" : "=g" (EFLAG_VAR)) // inline assemble


// extern variables
static struct list_elem* thread_tag;
extern struct task_struct* main_thread; // 主线程
extern struct list thread_ready_list; //就绪队列
extern struct list thread_all_list; //所有线程队列

static void pic_init(void) {
    outb(PIC_M_CTRL, 0x11);
    outb(PIC_M_DATA, 0x20);

    outb(PIC_M_DATA, 0x04);
    outb(PIC_M_DATA, 0x01);

    outb(PIC_S_CTRL, 0x11);
    outb(PIC_S_DATA, 0x28);

    outb(PIC_S_DATA, 0x02);
    outb(PIC_S_DATA, 0x01);

    outb(PIC_M_DATA, 0xfe);
    outb(PIC_S_DATA, 0xff);

    put_str("    pic init done!\n");
}

char* intr_name[IDT_DESC_CNT];
intr_handler idt_table[IDT_DESC_CNT]; // 处理中断的具体c语言程序
extern intr_entry_table[IDT_DESC_CNT];

struct gate_desc {
    uint16_t func_offset_low_word;
    uint16_t selector;
    uint8_t dcount;
    uint8_t attribute;
    uint16_t func_offeset_high_word;
};

static void make_idt_desc(struct gate_desc* p_gdesc, uint8_t attr, intr_handler function);
static struct gate_desc idt[IDT_DESC_CNT]; // IDT table IDT表

static void make_idt_desc(struct gate_desc* p_gdesc, uint8_t attr, intr_handler function) {
    p_gdesc->func_offset_low_word = (uint32_t)function & 0x0000FFFF; // low 32 bits
    p_gdesc->selector = SELECTOR_K_CODE;
    p_gdesc->dcount = 0;
    p_gdesc->attribute = attr;
    p_gdesc->func_offeset_high_word = (uint32_t)function & 0xFFFF0000; // high 32 bits
}
static void idt_desc_init(void) {
    int i;
    for(int i=0; i<IDT_DESC_CNT; i++) {
        make_idt_desc(&idt[i], IDT_DESC_ATTR_DPL0, intr_entry_table[i]);
    }
    put_str("    idt_desc_init done!\n");
}


static void general_intr_handler(uint8_t vec_nr) {
    if(vec_nr == 0x27 || vec_nr == 0x2f) {
        return;  // two interrupt vector which don't need to consider
    }

    // clean the screen and get enough information
    set_cursor(0);
    int cursor_pos = 0;

    while(cursor_pos<320) {
        put_char(' ');
        cursor_pos++;
    }
    set_cursor(0);

    put_str("\n!!!!    exception message begin    !!!!\n");
    set_cursor(88); // set to second line and second row

    put_str(intr_name[vec_nr]); // print interrupt vector information

    // Pagefault, print some useful information
    if(vec_nr == 14) {
        int page_fault_vaddr = 0;
        asm ("movl %%cr2, %0" : "=r" (page_fault_vaddr)); // store virtual address

        put_str("\n page fault addr is");put_int(page_fault_vaddr);
        put_str("\n    exception message end    \n");

        while(1);  // the interrupt is closed. which lead the system to always move silently
    } 
    //show the interrupt information here

    
    // put_str("int vector : 0x2");
    // char tmp = (char)(vec_nr + 'a');
    // put_char(tmp);
    // put_int(vec_nr);
    // put_char('\n');
}

static void exception_init(void) {
    int i;
    for(i=0; i<IDT_DESC_CNT; i++) {
        // init as void which is equ 000000
        idt_table[i] = general_intr_handler;
        intr_name[i] = "unknown";
    }

    // register intr name
    intr_name[0] = "#DE Divide Error";
    intr_name[1] = "#DB Debug Exception";
    intr_name[2] = "NMI Interrupt";
    intr_name[3] = "#BP Breakpoint Exception";
    intr_name[4] = "#OF Overflow Exception";
    intr_name[5] = "#BR BOUND Range Exceeded Exception";
    intr_name[6] = "#UD Invalid Opcode Exception";
    intr_name[7] = "#NM Device Not Available Exception";
    intr_name[8] = "#DF Double Fault Exception";
    intr_name[9] = "Coprocessor Segment Overrun";
    intr_name[10] = "#TS Invalid TSS Exception";
    intr_name[11] = "#NP Segment Not Present";
    intr_name[12] = "#SS Stack Fault Exception";
    intr_name[13] = "#GP General Protection Exception";
    intr_name[14] = "#PF Page-Fault Exception";
    // intr_name[15] 第15项是intel保留项，未使用
    intr_name[16] = "#MF x87 FPU Floating-Point Error";
    intr_name[17] = "#AC Alignment Check Exception";
    intr_name[18] = "#MC Machine-Check Exception";
    intr_name[19] = "#XF SIMD Floating-Point Exception";
}

void idt_init() {
    put_str("idt init start!\n");
    idt_desc_init();
    exception_init();
    pic_init(); // 8259A init, no need to understand
    
    uint64_t idt_operand  = ((sizeof(idt)-1) | (uint64_t)((uint32_t)idt << 16));
    // load idt!
    asm volatile("lidt %0" : : "m" (idt_operand));
    put_str("idt_init done!\n");
    // next thing is complex
}

// open the interrupt
// all these functions are used for open or close interrupt
enum intr_status intr_disable(void) {
    enum intr_status old_status;
    if(INTR_ON == intr_get_status()) {
        old_status = INTR_ON;
        asm volatile("cli" : : : "memory"); // cli set IF=0, close the interrupt
        return old_status;
    }
    else {
        old_status = INTR_OFF;
        return old_status;
    }
}
enum intr_status intr_enable(void) {
    enum intr_status old_status;
    if(INTR_ON == intr_get_status()) {
        old_status = INTR_ON;
        return old_status;
    }
    else {
        old_status = INTR_OFF;
        asm volatile("sti"); // cil set IF=1, open the interrupt
        return old_status;
    }
}

enum intr_status intr_get_status(void) {
    uint32_t eflags = 0;
    GET_EFLAGS(eflags);
    return (EFLAGS_IF&eflags)?INTR_ON:INTR_OFF;
}
enum intr_status intr_set_status(enum intr_status status) {
    return status & INTR_ON? intr_enable() : intr_disable();
}

void register_handler(uint8_t vector_no, intr_handler function) {
    idt_table[vector_no] = function; // point to function pointer
}

// now the schedule is below, it is very exciting!

void schedule() {
    // ensure that the interrupt is closed
    ASSERT(intr_get_status() == INTR_OFF);

    struct task_struct* cur = running_thread();

    if(cur->status == TASK_RUNNING) {
        ASSERT(!elem_find(&thread_ready_list, &cur->general_tag));
        list_append(&thread_ready_list, &cur->general_tag);
        cur->ticks = cur->priority;
        cur->status = TASK_READY;
    }
    else {
        // not need to add to the queue
    }

    ASSERT(!list_empty(&thread_ready_list));
    thread_tag = list_pop(&thread_ready_list);
    struct task_struct* next = elem2entry(struct task_struct, general_tag, thread_tag);
    next->status = TASK_RUNNING;

    switch_to(cur, next);
}