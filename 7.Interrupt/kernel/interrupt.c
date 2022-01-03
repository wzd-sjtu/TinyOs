#include "interrupt.h"
#include "stdint.h"
#include "global.h"
#include "io.h"

#define IDT_DESC_CNT 0x21

#define PIC_M_CTRL 0x20
#define PIC_M_DATA 0x21
#define PIC_S_CTRL 0xa0
#define PIC_S_DATA 0xa1
// init the pic 8259A


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
    put_str("int vector : 0x2");
    char tmp = (char)(vec_nr + 'a');
    // put_char(tmp);
    // put_int(vec_nr);
    put_char('\n');
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