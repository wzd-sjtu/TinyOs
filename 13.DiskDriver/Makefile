# rewrite version of MakeFile
BUILD_DIR = ./build
ENTRY_POINT = 0xc0001500
AS = nasm
CC = gcc
LD = ld
LIB = -I lib/ -I lib/kernel -I lib/user -I kernel/ -I device/ -I thread/ -I userprog/
ASFLAGS = -f elf32
CFLAGS = -Wall $(LIB) -m32 -c -fno-builtin -fno-stack-protector -W -Wstrict-prototypes -Wmissing-prototypes

LDFLAGS = -m elf_i386 -Ttext 0xc0001500 -e main
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/init.o $(BUILD_DIR)/interrupt.o \
	$(BUILD_DIR)/timer.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/print.o \
	$(BUILD_DIR)/debug.o $(BUILD_DIR)/memory.o $(BUILD_DIR)/string.o \
	$(BUILD_DIR)/bitmap.o $(BUILD_DIR)/thread.o $(BUILD_DIR)/list.o \
	$(BUILD_DIR)/switch.o $(BUILD_DIR)/sync.o $(BUILD_DIR)/console.o \
	$(BUILD_DIR)/keyboard.o $(BUILD_DIR)/ioqueue.o $(BUILD_DIR)/tss.o \
	$(BUILD_DIR)/process.o $(BUILD_DIR)/syscall-init.o $(BUILD_DIR)/syscall.o \
	$(BUILD_DIR)/stdio.o


ALL_DEPENDENCIES_H = device/timer.h device/console.h device/keyboard.h device/ioqueue.h\
					kernel/debug.h kernel/global.h kernel/init.h kernel/interrupt.h \
	kernel/memory.h \
					lib/stdint.h lib/string.h lib/stdio.h \
					lib/kernel/bitmap.h lib/kernel/io.h lib/kernel/print.h \
	lib/kernel/list.h \
					thread/thread.h thread/sync.h \
					userprog/tss.h userprog/userprog.h userprog/process.h \
					lib/user/syscall.h userprog/syscall-init.h

## compile for c code ## memory
$(BUILD_DIR)/main.o: kernel/main.c lib/kernel/print.h \
        lib/stdint.h kernel/init.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/init.o: kernel/init.c kernel/init.h lib/kernel/print.h \
        lib/stdint.h kernel/interrupt.h device/timer.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/interrupt.o: kernel/interrupt.c kernel/interrupt.h \
        lib/stdint.h kernel/global.h lib/kernel/io.h lib/kernel/print.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/timer.o: device/timer.c device/timer.h lib/stdint.h\
        lib/kernel/io.h lib/kernel/print.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/debug.o: kernel/debug.c kernel/debug.h \
        lib/kernel/print.h lib/stdint.h kernel/interrupt.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/string.o: lib/string.c lib/string.h lib/stdint.h kernel/global.h \
	lib/stdint.h kernel/debug.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/bitmap.o: lib/kernel/bitmap.c lib/kernel/bitmap.h \
    	kernel/global.h lib/stdint.h lib/string.h lib/stdint.h \
     	lib/kernel/print.h kernel/interrupt.h kernel/debug.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/memory.o: kernel/memory.c kernel/memory.h lib/stdint.h lib/kernel/bitmap.h \
   	kernel/global.h kernel/global.h kernel/debug.h lib/kernel/print.h \
	lib/kernel/io.h kernel/interrupt.h lib/string.h lib/stdint.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/thread.o: thread/thread.c thread/thread.h lib/stdint.h lib/kernel/list.h \
    	kernel/global.h lib/string.h lib/stdint.h kernel/debug.h \
     	kernel/interrupt.h lib/kernel/print.h kernel/memory.h \
      	lib/kernel/bitmap.h userprog/process.h thread/thread.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/list.o: lib/kernel/list.c lib/kernel/list.h kernel/global.h lib/stdint.h \
        kernel/interrupt.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/console.o: device/console.c device/console.h lib/stdint.h \
        lib/kernel/print.h thread/sync.h lib/kernel/list.h kernel/global.h \
     	thread/thread.h thread/thread.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/sync.o: thread/sync.c thread/sync.h lib/kernel/list.h kernel/global.h \
       	lib/stdint.h thread/thread.h lib/string.h lib/stdint.h kernel/debug.h \
	kernel/interrupt.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/keyboard.o: device/keyboard.c device/keyboard.h lib/kernel/print.h \
        lib/stdint.h kernel/interrupt.h lib/kernel/io.h device/ioqueue.h \
	thread/thread.h lib/kernel/list.h kernel/global.h thread/sync.h \
      	thread/thread.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/ioqueue.o: device/ioqueue.c device/ioqueue.h lib/stdint.h thread/thread.h \
        lib/kernel/list.h kernel/global.h thread/sync.h thread/thread.h kernel/interrupt.h \
        kernel/debug.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/tss.o: userprog/tss.c userprog/tss.h thread/thread.h lib/stdint.h \
    	lib/kernel/list.h kernel/global.h lib/string.h lib/stdint.h \
     	lib/kernel/print.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/process.o: userprog/process.c userprog/process.h thread/thread.h \
    	lib/stdint.h lib/kernel/list.h kernel/global.h kernel/debug.h \
     	kernel/memory.h lib/kernel/bitmap.h userprog/tss.h kernel/interrupt.h \
      	lib/string.h lib/stdint.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/syscall.o: lib/user/syscall.c lib/user/syscall.h lib/stdint.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/syscall-init.o: userprog/syscall-init.c userprog/syscall-init.h \
    	lib/stdint.h lib/user/syscall.h lib/kernel/print.h thread/thread.h \
     	lib/kernel/list.h kernel/global.h lib/kernel/bitmap.h kernel/memory.h \
	device/console.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/stdio.o: lib/stdio.c lib/stdio.h lib/stdint.h kernel/interrupt.h \
    	lib/stdint.h kernel/global.h lib/string.h lib/user/syscall.h lib/kernel/print.h
	$(CC) $(CFLAGS) $< -o $@
## compile for assemb code ##
$(BUILD_DIR)/kernel.o: kernel/kernel.S
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/print.o: lib/kernel/print.S
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/switch.o: thread/switch.S
	$(AS) $(ASFLAGS) $< -o $@



loader.bin:boot/loader.S 
	nasm -I boot/include/ -o build/loader.bin boot/loader.S
mbr.bin:boot/mbr.S
	nasm -I boot/include/ -o build/mbr.bin boot/mbr.S
## link all files ##
$(BUILD_DIR)/kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

all:loader.bin mbr.bin $(BUILD_DIR)/kernel.bin
	dd if=./build/loader.bin of=../../bochs/hd60M.img bs=512 count=4 seek=2 conv=notrunc
	dd if=./build/mbr.bin of=../../bochs/hd60M.img bs=512 count=1 conv=notrunc
	dd if=./build/kernel.bin of=../../bochs/hd60M.img bs=512 count=200  seek=9 conv=notrunc
	echo "write successfully!"

clean:
	rm build/*.o
	rm build/*.bin