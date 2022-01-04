#ifndef __LIB_IO_H
#define __LIB_IO_H
#include "stdint.h"

// write a byte into the port
static inline void outb(uint16_t port, uint8_t data) {
    // inline asmble language
    // uint8_t wzd;
    // asm volatile("outb %b0, %wl" :: "a" (data), "Nd" (port));
    asm volatile ( "outb %b0, %w1" : : "a" (data), "Nd" (port));
}
static inline void outsw(uint16_t port, const void* addr, uint32_t word_cnt) {
    asm volatile("cld; rep outsw" : "+S" (addr), "+c" (word_cnt) : "d" (port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    asm volatile("inb %w1, %b0" : "=a" (data) : "Nd" (port));
    return data;
}
static inline void insw(uint16_t port, void* addr, uint32_t word_cnt) {
    asm volatile("cld; rep insw" : "+D" (addr), "+c" (word_cnt) : "d" (port) : "memory");
    // inline assmble , which is no need to understand.
}
#endif