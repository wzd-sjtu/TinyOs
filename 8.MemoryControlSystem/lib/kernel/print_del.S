TI_GDT equ 0
RPL0 equ 0
SELECTOR_VIDEO equ (0x0003<<3) + TI_GDT + RPL0

[bits 32]
section .text

; put a str into the screen
global put_str
put_str:
    push ebx
    push ecx
    xor ecx, ecx
    mov ebx, [esp+12]
.goon:
    mov cl, [ebx]
    cmp cl, 0
    jz .str_over
    push ecx ; for put_char deliver parameters
    call put_char
    add esp, 4
    inc ebx
    jmp .goon
.str_over:
    pop ecx
    pop ebx
    ret
; put a char in stack into the screen
global put_char
put_char:
    pushad ; 备份32位寄存器环境
    mov ax, SELECTOR_VIDEO
    mov gs, ax ; 将视频段选择子送入gs寄存器

    ; 获取光标地址
    mov dx, 0x03d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x03d5
    in al, dx
    mov ah, al

    mov dx, 0x03d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x03d5
    in al, dx
    ; 此时，ax中存有光标的地址

    mov bx, ax
    mov ecx, [esp+36] ; the char in stack which is needed tp be printed

    cmp cl, 0xd
    jz .is_carriage_return ; CR and LF,统一理解为回车换行
    cmp cl, 0xa
    jz .is_line_feed

    cmp cl, 0x8
    jz .is_backspace

    jmp .put_other ;直接打印可见的字符

.is_backspace:
    dec bx
    shl bx, 1

    mov byte [gs:bx], 0x20
    inc bx
    mov byte [gs:bx], 0x07
    shr bx, 1
    jmp .set_cursor

.put_other:
    shl bx, 1
    mov [gs:bx], cl
    inc bx
    mov byte [gs:bx], 0x07
    shr bx, 1
    inc bx ;next cursor number
    cmp bx, 2000
    jl .set_cursor


.is_line_feed:
.is_carriage_return:
    xor dx, dx ; reg return to 0
    mov ax, bx
    mov si, 80

    div si

    sub bx, dx
.is_carriage_return_end:
    add bx, 80
    cmp bx, 2000
.is_line_feed_end:
    jl .set_cursor

.roll_screen:
    cld
    mov ecx, 960

    mov esi, 0xc00b80a0
    mov edi, 0xc00b8000
    rep movsd

    mov ebx, 3840
    mov ecx, 80
.cls:
    mov word [gs:ebx], 0x0720
    add ebx, 2
    loop .cls
    mov bx, 1920

.set_cursor:
    mov dx, 0x03d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x03d5
    mov al, bh
    out dx, al

    mov dx, 0x03d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x03d5
    mov al, bl
    out dx, al

.put_char_done:
    popad
    ret