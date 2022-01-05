# rewrite version of MakeFile
BUILD_DIR = ./build
ENTRY_POINT = 0xc0001500
AS = nasm
CC = gcc
LD = ld
LIB = -I lib/ -I lib/kernel -I lib/user -I kernel/ -I device/
ASFLAGS = -f elf32
CFLAGS = $(LIB) -m32 -c -fno-builtin -fno-stack-protector

LDFLAGS = -m elf_i386 -Ttext 0xc0001500 -e main
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/init.o $(BUILD_DIR)/interrupt.o \
	$(BUILD_DIR)/timer.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/print.o \
	$(BUILD_DIR)/debug.o $(BUILD_DIR)/memory.o $(BUILD_DIR)/string.o \
	$(BUILD_DIR)/bitmap.o $(BUILD_DIR)/thread.o $(BUILD_DIR)/list.o

ALL_DEPENDENCIES_H = device/timer.h \
					kernel/debug.h kernel/global.h kernel/init.h kernel/interrupt.h \
	kernel/memory.h \
					lib/stdint.h lib/string.h \
					lib/kernel/bitmap.h lib/kernel/io.h lib/kernel/print.h \
	lib/kernel/list.h
## compile for c code ##
$(BUILD_DIR)/main.o: kernel/main.c lib/kernel/print.h \
        lib/stdint.h kernel/init.h kernel/memory.h thread/thread.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/init.o: kernel/init.c kernel/init.h lib/kernel/print.h \
        lib/stdint.h kernel/interrupt.h device/timer.h kernel/memory.h
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

$(BUILD_DIR)/bitmap.o: lib/kernel/bitmap.c lib/kernel/bitmap.h lib/stdint.h lib/string.h \
	lib/kernel/print.h kernel/interrupt.h kernel/debug.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/string.o: lib/string.c lib/string.h kernel/global.h kernel/debug.h \
	lib/stdint.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/memory.o: kernel/memory.c kernel/memory.h lib/stdint.h \
	lib/kernel/print.h kernel/debug.h lib/string.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/thread.o: thread/thread.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/list.o: lib/kernel/list.c $(ALL_DEPENDENCIES_H)
	$(CC) $(CFLAGS) $< -o $@

## compile for assemb code ##
$(BUILD_DIR)/kernel.o: kernel/kernel.S
	$(AS) $(ASFLAGS) $< -o $@
$(BUILD_DIR)/print.o: lib/kernel/print.S
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