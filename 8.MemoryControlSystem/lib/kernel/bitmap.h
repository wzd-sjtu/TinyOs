#ifndef __LIB_KERNEL_BITMAP_H
#define __LIB_KERNEL_BITMAP_H
#include "global.h"
#define BIT_MAP_MASK 1


struct bitmap {
    uint32_t btmp_bytes_len;
    uint8_t* bits; // pointer for bitmap
};

void bitmap_init(struct bitmap* btmp);
int bitmap_scan_test(struct bitmap* btmp, uint32_t bit_idx);
int bitmap_scan(struct bitmap* btmp, uint32_t cnt);
void bitmap_set(struct bitmap* btmp, uint32_t bit_idx, int8_t value);

#endif