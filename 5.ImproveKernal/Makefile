# nasm -I include/ -o loader.bin loader.S
# nasm -I include/ -o mbr.bin mbr.S
# gcc -c -o kernal/main.o kernal/main.c   // 编译
# ld kernal/main.o -Ttext 0xc0001500 -e main -o kernal/kernal.bin    // 链接重定位

# dd if=./loader.bin of=./hd60M.img bs=512 count=4 seek=2 conv=notrunc
# dd if=./mbr.bin of=./hd60M.img bs=512 count=1 conv=notrunc
# dd if=kernal.bin of=./hd60M.img bs=512 count=200  seek=9 conv=notrunc

loader.bin:loader.S 
	nasm -I include/ -o build/loader.bin loader.S
mbr.bin:mbr.S 
	nasm -I include/ -o build/mbr.bin mbr.S

print.o:lib/kernel/print.S
	nasm -f elf32 -o build/print.o lib/kernel/print.S
main.o:kernal/main.c lib/kernel/print.h print.o
	gcc -I lib/kernel/ -m32 build/print.o kernal/main.c -c -o build/main.o

kernal.bin:main.o print.o
	ld -m elf_i386 -Ttext 0xc0001500 -e main -o build/kernal.bin build/print.o build/main.o
all:loader.bin mbr.bin kernal.bin
	dd if=./build/loader.bin of=../../bochs/hd60M.img bs=512 count=4 seek=2 conv=notrunc
	dd if=./build/mbr.bin of=../../bochs/hd60M.img bs=512 count=1 conv=notrunc
	dd if=./build/kernal.bin of=../../bochs/hd60M.img bs=512 count=200  seek=9 conv=notrunc
	echo "write successfully!"
clean:
	rm build/*.o
	rm build/*.bin