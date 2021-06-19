; 代码功能：在屏幕上打印 1 MBR ，背景色为黑色，前景色为绿色
; 主引导程序MBR

; 改进MBR，直接和显卡进行交互
%include "boot.inc"

SECTION MBR vstart=0x7c00
    ; 初始化所有寄存器
    mov ax,cs
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov fs,ax
    mov sp,0x7c00
    ; 显存基地址
    mov ax,0xb800
    mov gs,ax

; 清屏，使用的是固定的系统中断，不需要理解
    mov ax, 0x600
    mov bx, 0x700
    mov cx, 0
    mov dx, 0x184f

    int 0x10
    
    mov byte [gs:0x00],'1'
    ; 0xA4 A表示绿色背景闪烁，4表示前景色为红色
    mov byte [gs:0x01],0xA4

    mov byte [gs:0x02],' '
    mov byte [gs:0x03],0xA4

    mov byte [gs:0x04],'M'
    mov byte [gs:0x05],0xA4

    mov byte [gs:0x06],'B'
    mov byte [gs:0x07],0xA4

    mov byte [gs:0x08],'R'
    mov byte [gs:0x09],0xA4

    mov eax,LOADER_START_SECTOR ; 起始扇区LBA地址
    mov bx,LOADER_BASE_ADDR ; loader程序写入内存地址
    mov cx,4 ; loader程序超过了512byte，为了避免将来再次修改，修改为4个扇区
    call rd_disk_m_16 ; 从磁盘读出内容，从而写入内存

    jmp LOADER_BASE_ADDR ; 写入完成后，直接跳转到目标地址

rd_disk_m_16:
    mov esi,eax ; 备份LBA号
    mov di,cx

    ; 设置要读取的扇区数
    mov dx,0x1f2 ; primary通道sector count寄存器
    mov al,cl ; cl就是cx的低位
    out dx,al ; 内容写入目标端口
    mov eax,esi ; 恢复eax寄存器

    ; 写入LBA地址
    mov dx,0x1f3
    out dx,al

    mov cl,8
    shr eax,cl 
    mov dx,0x1f4
    out dx,al 

    shr eax,cl 
    mov dx,0x1f5
    out dx,al 

    shr eax,cl 
    and al,0x0f ; &位运算
    or al,0xe0 ; 1110,表示为lba模式
    mov dx,0x1f6
    out dx,al 

    ; 0x1f7端口写入读命令，0x20
    mov dx,0x1f7
    mov al,0x20 
    out dx,al 

    ;检测硬盘状态
.not_ready:
    nop
    in al,dx 
    and al,0x88 ; 看就绪位是否为1
    cmp al,0x08 
    jnz .not_ready ; 未准备好，循环等待

    ; 从0x1f0读取数据
    mov ax,di ; 扇区数
    mov dx,256
    mul dx
    mov cx,ax 

    ; 每次读入16byte，则可以计算出需要的空间

    mov dx,0x1f0 
.go_on_read:
    in ax,dx 
    mov [bx],ax ; 向内存写入数据
    add bx,2
    loop .go_on_read
    ret 


    times 510-($-$$) db 0
    db 0x55, 0xaa ; MBR的结束标志