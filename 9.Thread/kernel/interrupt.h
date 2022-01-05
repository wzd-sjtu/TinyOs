#ifndef __INTERRUPT_H
#define __INTERRUPT_H

#include "stdint.h"

typedef void* intr_handler;
void idt_init(void);


enum intr_status {
    INTR_OFF, // 0 CLOSE THE INTERRUPT
    INTR_ON   // 1 OPEN THE INTERRUPT
};

enum intr_status intr_get_status(void);
enum intr_status intr_set_status(enum intr_status);
enum intr_status intr_enable(void);
enum intr_status intr_disable(void);

void register_handler(uint8_t vector_no, intr_handler function);

#endif