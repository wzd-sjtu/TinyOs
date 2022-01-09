# rewrite version of MakeFile
BUILD_DIR = ./build
ENTRY_POINT = 0xc0001500
AS = nasm
CC = gcc
LD = ld
LIB = -I lib/ -I lib/kernel -I lib/user -I kernel/ -I device/ -I thread/ -I userprog/
ASFLAGS = -f elf32
CFLAGS = $(LIB) -m32 -c -fno-builtin -fno-stack-protector

LDFLAGS = -m elf_i386 -Ttext 0xc0001500 -e main
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/init.o $(BUILD_DIR)/interrupt.o \
	$(BUILD_DIR)/timer.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/print.o \
	$(BUILD_DIR)/debug.o $(BUILD_DIR)/memory.o $(BUILD_DIR)/string.o \
	$(BUILD_DIR)/bitmap.o $(BUILD_DIR)/thread.o $(BUILD_DIR)/list.o \
	$(BUILD_DIR)/switch.o $(BUILD_DIR)/sync.o $(BUILD_DIR)/console.o \
	$(BUILD_DIR)/keyboard.o $(BUILD_DIR)/ioqueue.o $(BUILD_DIR)/tss.o \
	$(BUILD_DIR)/process.o

ALL_DEPENDENCIES_H = device/timer.h device/console.h device/keyboard.h device/ioqueue.h\
					kernel/debug.h kernel/global.h kernel/init.h kernel/interrupt.h \
	kernel/memory.h \
					lib/stdint.h lib/string.h \
					lib/kernel/bitmap.h lib/kernel/io.h lib/kernel/print.h \
	lib/kernel/list.h \
					thread/thread.h thread/sync.h \
					userprog/tss.h userprog/userprog.h userprog/process.h
## compile for c code ##
$(BUILD_DIR)/main.o: kernel/main.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/init.o: kernel/init.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/interrupt.o: kernel/interrupt.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/timer.o: device/timer.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/debug.o: kernel/debug.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/bitmap.o: lib/kernel/bitmap.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/string.o: lib/string.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/memory.o: kernel/memory.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/thread.o: thread/thread.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/sync.o: thread/sync.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/list.o: lib/kernel/list.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/console.o: device/console.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/keyboard.o: device/keyboard.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/ioqueue.o: device/ioqueue.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/tss.o: userprog/tss.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/process.o: userprog/process.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@	
## compile for assemb code ##
$(BUILD_DIR)/kernel.o: kernel/kernel.S $(ALL_DEPENDENCIES_H)
	$(AS) $(ASFLAGS) $< -o $@
$(BUILD_DIR)/print.o: lib/kernel/print.S $(ALL_DEPENDENCIES_H)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/switch.o: thread/switch.S $(ALL_DEPENDENCIES_H)
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