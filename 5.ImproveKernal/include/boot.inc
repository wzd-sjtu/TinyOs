; 配置文件区 loader 和 Kernal
LOADER_BASE_ADDR equ 0x900
LOADER_START_SECTOR equ 0x2
PAGE_DIR_TABLE_POS equ 0x100000 ; 这里居然有24位地址，超出低端1MB空间的第一个字节 0xfffff+1 = 0x100000

; 这里配置几乎所有用到的文件
; 逆着光写代码，这也太uncomfortable了

; GDT描述符属性
DESC_G_4K equ 1_00000000000000000000000b ; 23位 G为1，表示粒度是4KB
DESC_D_32 equ 1_0000000000000000000000b  ; 22位 D/B段，区别16位or32位,设置为1，表示是32位
DESC_L    equ 0_000000000000000000000b   ; 21位 L位，属于保留位，64位or32位编程，在这里是32位
DESC_AVL  equ 0_00000000000000000000b    ; 20位 AVL位，操作系统的馈赠，保留位

DESC_LIMIT_CODE2 equ 1111_0000000000000000b ; 19-16位，段界限第二部分
DESC_LIMIT_DATA2 equ DESC_LIMIT_CODE2
DESC_LIMIT_VIDEO2 equ 0000_0000000000000000b ; 此字段的意义不明

DESC_P    equ  1_000000000000000b        ; 15位 P位，段是否存在？置为1
DESC_DPL_0 equ 00_0000000000000b         ; 14-13位，表示DPL权限，可以取0/1/2/3
DESC_DPL_1 equ 01_0000000000000b 
DESC_DPL_2 equ 10_0000000000000b 
DESC_DPL_3 equ 11_0000000000000b 

DESC_S_CODE equ 1_000000000000b          ; 12位，表示是系统段还是数据段 系统段是硬件代码的运行环境，我们这里是代码段
DESC_S_DATA equ 1_000000000000b          ; 12位
DESC_S_sys  equ 0_000000000000b          ; 12位，表示系统段，硬件代码

DESC_TYPE_CODE equ 1000_00000000b        ; 11-8位，表示数据段或者代码段的权限 可执行、不可访问 
DESC_TYPE_DATA equ 0010_00000000b        ; 11-8位，表示数据段不可执行，但是可写

DESC_CODE_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L + DESC_AVL + DESC_LIMIT_CODE2 + DESC_P + DESC_DPL_0 + DESC_S_CODE + DESC_TYPE_CODE + 0x00
DESC_DATA_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L + DESC_AVL + DESC_LIMIT_DATA2 + DESC_P + DESC_DPL_0 + DESC_S_DATA + DESC_TYPE_DATA + 0x00
DESC_VIDEO_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L + DESC_AVL + DESC_LIMIT_VIDEO2 + DESC_P + DESC_DPL_0 + DESC_S_DATA + DESC_TYPE_DATA + 0x0b ; 这里书上有bug

; 以上设置了GDT描述符的高24位，剩下的位是真正的地址了
; 下面设置选择子的属性
RPL0 equ 00b
RPL1 equ 01b 
RPL2 equ 10b 
RPL3 equ 11b 
TI_GDT equ 000b 
TI_LDT equ 100b 


; 页表的相关属性
PG_P equ 1b      ; P表示是否存在于内存，设置为1，表示在内存里面？
PG_RW_R equ 00b  ; 用户读
PG_RW_W equ 10b  ; 用户写
PG_US_S equ 000b ; 系统内容
PG_US_U equ 100b ; 用户内容

KERNAL_START_SECTOR equ 0x9
KERNAL_BIN_BASE_ADDR equ 0x70000 ; also in true mode 
KERNAL_ENTER_POINT equ 0xc0001500
PT_NULL equ 0