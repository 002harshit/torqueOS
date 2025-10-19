TOOLCHAIN ?=$(HOME)/opt/cross_i386-elf/bin/i386-elf-
CC=$(TOOLCHAIN)gcc
AS=$(TOOLCHAIN)as
LD=$(TOOLCHAIN)ld

TARGET=torque

OBJS = \
kernel/loader.s.o \
kernel/kmain.o \
kernel/gdt.s.o \
kernel/gdt.o \
kernel/idt.s.o \
kernel/idt.o \
kernel/paging.s.o \
kernel/paging.o \
kernel/kheap.o \
kernel/io.o \
driver/framebuffer.o \
driver/serial.o \
driver/timer.o \
driver/keyboard.o \
common/printf.o

LIBCRANK_SOURCES = $(wildcard libcrank/*.c libcrank/*/*.c)
LIBCRANK_OBJS = $(LIBCRANK_SOURCES:.c=.o)

HEADERS = \
kernel/io.h \
kernel/gdt.h \
kernel/idt.h \
kernel/paging.h \
kernel/kheap.h \
driver/framebuffer.h \
driver/serial.h \
driver/timer.h \
driver/keyboard.h


CFLAGS =
CFLAGS += -std=gnu99 -ffreestanding -O2 -Wall -Wextra -g
CFLAGS += -I./
CFLAGS +=-masm=intel

QEMU_IMG = qemu_disk.qcow2

all: $(TARGET).iso

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

%.s.o: %.asm
	yasm -f elf32 $< -o $@

$(TARGET).elf: $(OBJS) kernel/linker.ld $(LIBCRANK_OBJS)
	$(LD) -T kernel/linker.ld -melf_i386 $(OBJS) $(LIBCRANK_OBJS) -o $(TARGET).elf

iso/boot/grub/stage2_eltorito iso/boot/grub/menu.lst: stage2_eltorito menu.lst
	mkdir -p iso/boot/grub
	cp stage2_eltorito iso/boot/grub
	cp menu.lst iso/boot/grub

echoo:
	echo $(LIBCRANK_SOURCES)

$(TARGET).iso: $(TARGET).elf iso/boot/grub/stage2_eltorito iso/boot/grub/menu.lst
	cp $(TARGET).elf iso/boot
	genisoimage -R                  \
	-b boot/grub/stage2_eltorito    \
	-no-emul-boot                   \
	-boot-load-size 4               \
	-A os                           \
	-input-charset utf8             \
	-quiet                          \
	-boot-info-table                \
	-o $(TARGET).iso                \
	iso

bochs: $(TARGET).iso bochsrc.txt
	bochs -f bochsrc.txt -q

$(QEMU_IMG):
	qemu-img create -f qcow2 $(QEMU_IMG) 1G

qemu: $(TARGET).iso $(QEMU_IMG)
	qemu-system-i386 \
	-m 1G \
	-smp 2 \
	-boot d \
	-cdrom $(TARGET).iso \
	-drive file=$(QEMU_IMG),format=qcow2 \
	-netdev user,id=net0,hostfwd=tcp::2222-:22 \
	-device e1000,netdev=net0 \
	-display sdl,show-cursor=on \

clean:
	rm -rf $(OBJS) $(LIBCRANK_OBJS) $(TARGET).elf $(TARGET).iso com1.out bochslog.txt bx_enh_dbg.ini *.s *.o *.out *.elf *.iso iso/
	
.PHONY: clean bochs qemu echoo
