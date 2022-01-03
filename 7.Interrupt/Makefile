# nasm -I include/ -o loader.bin loader.S
# nasm -I include/ -o mbr.bin mbr.S
# gcc -c -o kernal/main.o kernal/main.c   // 编译
# ld kernal/main.o -Ttext 0xc0001500 -e main -o kernal/kernal.bin    // 链接重定位

# dd if=./loader.bin of=./hd60M.img bs=512 count=4 seek=2 conv=notrunc
# dd if=./mbr.bin of=./hd60M.img bs=512 count=1 conv=notrunc
# dd if=kernal.bin of=./hd60M.img bs=512 count=200  seek=9 conv=notrunc

# 1. mbr and loader 
loader.bin:boot/loader.S 
	nasm -I boot/include/ -o build/loader.bin boot/loader.S
mbr.bin:boot/mbr.S
	nasm -I boot/include/ -o build/mbr.bin boot/mbr.S

# 2.print.o
print.o:lib/kernel/print.S
	nasm -f elf32 -o build/print.o lib/kernel/print.S
# 3.kernel.o
kernel.o:kernel/kernel.S
	nasm -f elf32 -o build/kernel.o kernel/kernel.S
# 4.main.o
main.o:kernel/main.c lib/kernel/print.h 
	gcc -I lib/kernel/ -I lib/ -I kernel/ -m32 kernel/main.c -c -fno-builtin -o build/main.o
# 5.interrupt.o
interrupt.o:kernel/interrupt.c
	gcc -I lib/kernel/ -I lib/ -I kernel/ -m32 build/kernel.o kernel/interrupt.c -fno-stack-protector -c -fno-builtin -o build/interrupt.o

# 5.5 timer.o
timer.o:device/timer.c
	gcc -I lib/kernel -m32 -c -o build/timer.o device/timer.c

# 6.init.o
init.o:kernel/init.c
	gcc -I lib/kernel/ -I lib/ -I kernel/ -m32 kernel/init.c -c -fno-builtin -o build/init.o

# 7.link all of them
kernel.bin:print.o kernel.o main.o init.o interrupt.o timer.o
	ld -m elf_i386 -Ttext 0xc0001500 -e main -o build/kernel.bin build/main.o build/print.o build/kernel.o build/init.o build/interrupt.o build/timer.o

# ld -m elf_i386 -Ttext 0xc0001500 -e main -o build/kernel.bin build/main.o build/print.o build/kernel.o build/init.o build/interrupt.o

all:loader.bin mbr.bin kernel.bin
	dd if=./build/loader.bin of=../../bochs/hd60M.img bs=512 count=4 seek=2 conv=notrunc
	dd if=./build/mbr.bin of=../../bochs/hd60M.img bs=512 count=1 conv=notrunc
	dd if=./build/kernel.bin of=../../bochs/hd60M.img bs=512 count=200  seek=9 conv=notrunc
	echo "write successfully!"
clean:
	rm build/*.o
	rm build/*.bin