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
void idt_init() {
    put_str("idt init start!\n");
    idt_desc_init();
    pic_init(); // 8259A init, no need to understand

    uint64_t idt_operand  = ((sizeof(idt)-1) | (uint64_t)((uint32_t)idt << 16));
    // load idt!
    asm volatile("lidt %0" : : "m" (idt_operand));
    put_str("idt_init done!\n");
    // next thing is complex
}