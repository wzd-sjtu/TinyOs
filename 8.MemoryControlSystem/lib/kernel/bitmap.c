#include "bitmap.h"
#include "stdint.h"
#include "string.h"
#include "print.h"
#include "interrupt.h"
#include "debug.h" // for ASSERT

void bitmap_init(struct bitmap* btmp) {
    memset(btmp->bits, 0, btmp->btmp_bytes_len); // all set to zero
}
// judge whether the idc bit is 0 or 1
bool bitmap_scan_test(struct bitmap* btmp, uint32_t bit_idx) {
    uint32_t byte_idx = bit_idx / 8;
    uint32_t bit_odd = bit_idx % 8;
    return (btmp->bits[byte_idx] & (BIT_MAP_MASK << bit_odd));
}
// find continue cnt bits for usage
int bitmap_scan(struct bitmap* btmp, uint32_t cnt) {
    uint32_t idx_byte = 0;

    while((0xff == btmp->bits[idx_byte]) &&(idx_byte < btmp->btmp_bytes_len)) {
        idx_byte++;
    }

    // first bytes?
    ASSERT(idx_byte < btmp->btmp_bytes_len); // must be less than? somehow unreason
    if(idx_byte == btmp->btmp_bytes_len) {
        return -1;
    }

    int idx_bit = 0;

    while((uint8_t)(BIT_MAP_MASK << idx_bit) & btmp->btmp_bits[idx_byte]) {
        idx_bit++;
    }
    int bit_idx_start = idx_byte*8 + idx_bit;
    if(cnt==1) {
        return bit_idx_start;
    }

    uint32_t bit_left = (btmp->btmp_bytes_len * 8 - bit_idx_start);
    uint32_t next_bit = bit_idx_start + 1;
    uint32_t count = 1;

    bit_idx_start = -1;
    while(bit_left-- > 0) {
        if(!bitmap_scan_test(btmp, next_bit)) {
            count++;
        }
        else {
            count = 0;
        }
        if(count == cnt) {
            bit_idx_start = next_bit - cnt + 1;
            break;
        }
        next_bit++;
    }
    // continue cnt bits's location
    return bit_idx_start;
}
void bitmap_set(struct bitmap* btmp, uint32_t bit_idx, int8_t value) {
    ASSERT((value==0)||(value==1));
    uint32_t byte_idx = bit_idx/8;
    uint32_t bit_odd = bit_idx%8;

    if(value) {
        btmp->bits[byte_idx] |= (BIT_MAP_MASK << bit_odd);
    }
    else {
        btmp->bits[byte_idx] &= ~(BIT_MAP_MASK << bit_odd); // set to zero
    }
}