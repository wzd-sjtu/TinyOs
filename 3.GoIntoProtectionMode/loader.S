%include "boot.inc"

section loader vstart=LOADER_BASE_ADDR ;加载地址已经固定为0x900
LOADER_STACK_TOP equ LOADER_BASE_ADDR
jmp loader_start 

; GDT是为了内核模式寻址才设立的数据结构，这点要谨记
; 定义全局描述符表GDT dd伪指令，define double-word，定义双字变量
; 小端字节序，所以选择前面写低位，后面写高位
GDT_BASE:   dd 0x00000000 ;16位地址
            dd 0x00000000
CODE_DESC:  dd 0x0000FFFF  ; from 31-0 bits location
            dd DESC_CODE_HIGH4 ; from 63-32 bits location 
DATA_STACK_DESC: dd 0x0000FFFF
                 dd DESC_DATA_HIGH4
VIDEO_DESC: dd 0x80000007;limit=(0xbffff-0xb80000)/4k=0x7 show memory GDT element
            dd DESC_VIDEO_HIGH4 ; 4代表4个byte，32位

GDT_SIZE equ $-GDT_BASE
GDT_LIMIT equ GDT_SIZE-1
times 60 dq 0 ; 保存60*8字节的空间，来存储多余的GDT表项，每一个描述符表项大小为8字节

; selector is used to select the basic location 
SELECTOR_CODE equ (0x0001<<3) + TI_GDT + RPL0
SELECTOR_DATA equ (0x0002<<3) + TI_GDT + RPL0
SELECTOR_VIDEO equ (0x0003<<3) + TI_GDT + RPL0 ;RPL特权级为0，使用TI_GDT内存描述符表，自动对齐成16位

; GDT全局选择符表指针，专门用来把数据加载到内存的，提供给lgdt加载GDT到gdtr寄存器
gdt_ptr dw GDT_LIMIT
        dd GDT_BASE

loadermsg db '2 loader in real.'

; gs寄存器在前面已经被设置过了
; 也就是在主引导程序中被设置过

; 正式开始程序引入
loader_start:

        ; 向实模式告别，在实模式下打印一个字符串
        mov sp, LOADER_BASE_ADDR
        mov bp, loadermsg ; ES:SP->字符串地址
        mov cx, 17
        mov ax, 0x1301
        mov bx, 0x001f
        mov dx, 0x1800
        int 0x10 ; BIOS中断向屏幕写入信息，表示即将进入保护模式

        ; 准备进入保护模式

        ; 打开A20地址总线
        in al, 0x92
        or al, 0000_0010B
        out 0x92,al 

        ; 加载GDT
        lgdt [gdt_ptr]

        ; CR0第0位置置为1
        mov eax, cr0 
        or eax, 0x00000001
        mov cr0, eax 

        jmp dword SELECTOR_CODE:p_mode_start ; 刷新流水线,delete all things in data?

[bits 32] 
; 保护模式程序
p_mode_start:
        mov ax, SELECTOR_DATA
        mov ds, ax
        mov es, ax
        mov ss, ax
        mov esp,LOADER_STACK_TOP
        mov ax, SELECTOR_VIDEO ; 之前就是这个VIDEO有问题，所以会导致打印出问题，书本中没有将地址完全写对
        mov gs, ax 

        mov byte [gs:160], 'P'
        ; the code of computer system should be here.
        jmp $ ; 程序悬停
