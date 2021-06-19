%include "boot.inc"

section loader vstart=LOADER_BASE_ADDR ;加载地址已经固定为0x900
LOADER_STACK_TOP equ LOADER_BASE_ADDR
jmp loader_start 

; GDT是为了内核模式寻址才设立的数据结构，这点要谨记
; 定义全局描述符表GDT dd伪指令，define double-word，定义双字变量
; 小端字节序，所以选择前面写低位，后面写高位
GDT_BASE:   dd 0x00000000 ;16位地址
            dd 0x00000000
CODE_DESC:  dd 0x0000FFFF
            dd DESC_CODE_HIGH4
DATA_STACK_DESC: dd 0x0000FFFF
                 dd DESC_DATA_HIGH4
VIDEO_DESC: dd 0x80000007;limit=(0xbffff-0xb80000)/4k=0x7
            dd DESC_VIDEO_HIGH4 ; 4代表4个byte，32位

GDT_SIZE equ $-GDT_BASE
GDT_LIMIT equ GDT_SIZE-1
times 60 dq 0 ; 保存60*8字节的空间，来存储多余的GDT表项，每一个描述符表项大小为8字节

; 构建选择子，看的太快了，整得人有点懵逼
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

        jmp dword SELECTOR_CODE:p_mode_start ; 刷新流水线

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
        
        ; 下面正式开启分页机制，现在头有点晕了，我裂开
        call setup_page
        sgdt [gdt_ptr]
        
        mov ebx, [gdt_ptr+2]
        or dword [ebx+0x18+4], 0xc0000000

        ; 经过固定操作，我们正式开启了分页，此时的地址全部是虚拟地址
        ; 当然，这里没有涉及页表替换等算法，具体如何替换可见操作系统课程书籍，不需要在此处考虑！



; 空间清零
setup_page:
        ; 首先把页目录占用空间清零
        mov ecx, 4096
        mov esi, 0
.clear_page_dir:
        mov byte [PAGE_DIR_TABLE_POS + esi], 0
        inc esi ; 自加操作
        loop clear_page_dir
; 创建页目录项
.create_pde:
        mov eax, PAGE_DIR_TABLE_POS ; 这里是实模式，写的都是真实地址
        add eax, 0x1000  ; 此时，eax为第一个页表的位置及属性
        mov ebx, eax 

        ; 表示所有特权级都可访问
        or eax, PG_US_U | PG_RW_W | PG_P 
        mov [PAGE_DIR_TABLE_POS + 0x0], eax ; eax是32位寄存器，页表目录项第0项，0x 0100 1000
        mov [PAGE_DIR_TABLE_POS + 0xc00], eax ; 分别存入0和768项

        sub eax, 0x1000
        mov [PAGE_DIR_TABLE_POS + 4092], eax ; 在最后一个页目录项中写入页表自己的物理地址，目的是将来动态操作页表

        ; 下面创建页表
        mov ecx, 256 ; 1MB的低端内存
        mov esi, 0
        mov edx, PG_US_U | PG_RW_W | PG_P 
.create_pte 
        ; ebx上方已经赋值，指向第一个页表的地址
        mov [ebx+esi*4], edx 
        add edx, 4096 ; 每个页表1024个页表项，也就是4096byte，每个页表项4byte
        inc esi 
        loop .create_pte 


        ; 创建内核其它页表的pde
        mov eax, PAGE_DIR_TABLE_POS
        add eax, 0x2000
        or eax, PG_US_U | PG_EW_W | PG_P 
        mov ebx, PAGE_DIR_TABLE_POS
        mov ecx, 254
        mov esi, 769
.create_kernal_pde:
        mov [ebx+esi*4], eax 
        inc esi 
        add eax, 0x1000
        loop .create_kernal_pde
        ret

        ; 至此完成整个内核的creation？