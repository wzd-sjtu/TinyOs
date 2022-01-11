#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "syscall.h"

#ifndef NULL
#define NULL 0
#endif


#define va_start(ap, v) ap = (va_list)(&v)
#define va_arg(ap, t) *((t*)(ap += 4))  // in fact, it is parameters in stack
#define va_end(ap) ap=NULL // clear the ap

static void itoa(uint32_t value, char** buf_ptr_addr, uint8_t base) {
    uint32_t m = value % base; // two exchanged to eight dimentions 
    uint32_t i = value/base;
    if(i) {
        itoa(i, buf_ptr_addr, base);
    }
    if(m<10) {
        *((*buf_ptr_addr)++) = m + '0';
    }
    else if(m>=10) { // firstly into a important pointer
        *((*buf_ptr_addr)++) = m - 10 + 'A'; // if m==10, then it is the same as A! yes
    }
}

uint32_t vsprintf(char* str, const char* format, va_list ap) {
    char* buf_ptr = str;
    const char* index_ptr = format; // this is for format, %d which is always string
    char index_char = *index_ptr; 
    int32_t arg_int;

    while(index_char) {
        if(index_char != '%') { 
            *(buf_ptr++) = index_char;
            index_char = *(++index_ptr);
            continue;
        }
        index_char = *(++index_ptr); //jump the %
        switch(index_char) {
            case 'x':
                arg_int = va_arg(ap, int); // ap is the parameters in stack
                itoa(arg_int, &buf_ptr, 16);
                index_char = *(++index_ptr);
                break;
        }
    } 
    return strlen(str);
}

// multi parameters completence of the function
uint32_t printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buf[1024] = {0};
    vsprintf(buf, format, args); // how many number of stack is here?
    va_end(args);
    return write(buf);  // firstly simulate the format, then printf it.
}