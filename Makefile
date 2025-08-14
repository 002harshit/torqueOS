all: set_grub torque.iso

CROSS_COMPILER_PREFIX?=$(HOME)/opt/cross_i386-elf/bin/i386-elf-
GCC=$(CROSS_COMPILER_PREFIX)gcc
LD=$(CROSS_COMPILER_PREFIX)ld

kernel_loader.o: kernel_loader.asm
	yasm -f elf32 kernel_loader.asm -o kernel_loader.o

test.o: test.c
	$(GCC) -c test.c -O3

framebuffer.o: driver/framebuffer.c
	$(GCC) -c driver/framebuffer.c -O3

serial.o: driver/serial.c
	$(GCC) -c driver/serial.c -O3


printf.o: lib/printf.c
	$(GCC) -c lib/printf.c -O3

io.o: io.c
	$(GCC) -c io.c -masm=intel -O3

kernel.elf: kernel_loader.o linker.ld test.o io.o framebuffer.o printf.o serial.o
	$(LD) -T linker.ld -melf_i386 kernel_loader.o test.o framebuffer.o printf.o serial.o io.o -o kernel.elf

set_grub: iso/boot/grub

iso/boot/grub: stage2_eltorito menu.lst
	mkdir -p iso/boot/grub
	cp stage2_eltorito iso/boot/grub
	cp menu.lst iso/boot/grub

torque.iso: kernel.elf
	cp kernel.elf iso/boot
	genisoimage -R                  \
	-b boot/grub/stage2_eltorito    \
	-no-emul-boot                   \
	-boot-load-size 4               \
	-A os                           \
	-input-charset utf8             \
	-quiet                          \
	-boot-info-table                \
	-o torque.iso                   \
	iso

bochs: torque.iso bochsrc.txt
	bochs -f bochsrc.txt -q

clean:
	rm -rf *.o kernel.elf *.iso iso
	
.PHONY: clean bochs
