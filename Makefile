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
kernel/kalloc.o \
kernel/io.o \
driver/vga.o \
driver/serial.o \
driver/timer.o \
driver/keyboard.o \
demos/spinning_donut.o \

HEADERS = \
kernel/io.h \
kernel/gdt.h \
kernel/idt.h \
kernel/kalloc.h \
driver/vga.h \
driver/serial.h \
driver/timer.h \
driver/keyboard.h

LIBCRANK_TARGET=libcrank.a
LIBCRANK_DIR=libcrank

LIBCRANK_OBJS = \
$(LIBCRANK_DIR)/math/basic.o \
$(LIBCRANK_DIR)/math/trig.o \
$(LIBCRANK_DIR)/math/float.o \
$(LIBCRANK_DIR)/math/mat4.o \
$(LIBCRANK_DIR)/math/vec2.o \
$(LIBCRANK_DIR)/math/vec3.o \
$(LIBCRANK_DIR)/std/memcmp.o \
$(LIBCRANK_DIR)/std/memcpy.o \
$(LIBCRANK_DIR)/std/memset.o \
$(LIBCRANK_DIR)/std/printf.o \
$(LIBCRANK_DIR)/string/strcat.o \
$(LIBCRANK_DIR)/string/strchr.o \
$(LIBCRANK_DIR)/string/strcmp.o \
$(LIBCRANK_DIR)/string/strcpy.o  \
$(LIBCRANK_DIR)/string/strlen.o \
$(LIBCRANK_DIR)/string/strncat.o \
$(LIBCRANK_DIR)/string/strncmp.o \
$(LIBCRANK_DIR)/string/strncpy.o \
$(LIBCRANK_DIR)/string/strrchr.o \
$(LIBCRANK_DIR)/string/strstr.o \
$(LIBCRANK_DIR)/string/strtok.o

LIBCRANK_HEADERS = \
$(LIBCRANK_DIR)/types.h \
$(LIBCRANK_DIR)/std.h \
$(LIBCRANK_DIR)/string.h \
$(LIBCRANK_DIR)/math.h

CFLAGS ?=
CFLAGS += -std=gnu99 -ffreestanding -nostdlib -O0 -Wall -Wextra -g -ggdb
CFLAGS += 
CFLAGS += -I./
CFLAGS +=-masm=intel

QEMU_IMG = qemu_disk.qcow2

ISO_DIR = iso

all: $(TARGET).iso

$(LIBCRANK_TARGET): $(LIBCRANK_OBJS)
	$(AR) rcs $(LIBCRANK_TARGET) $(LIBCRANK_OBJS)

%.o: %.c $(HEADERS) $(LIBCRANK_HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

%.s.o: %.asm
	yasm -f elf32 $< -o $@

$(TARGET).elf: $(OBJS) $(LIBCRANK_TARGET) kernel/linker.ld
	$(LD) -T kernel/linker.ld -melf_i386 $(OBJS) $(LIBCRANK_TARGET) -o $(TARGET).elf

$(ISO_DIR)/boot/grub/grub.cfg: grub.cfg
	mkdir -p $(ISO_DIR)/boot/grub
	cp grub.cfg $(ISO_DIR)/boot/grub

$(TARGET).iso: $(TARGET).elf $(ISO_DIR)/boot/grub/grub.cfg
	cp $(TARGET).elf iso/boot
	grub-mkrescue -o $(TARGET).iso $(ISO_DIR)/

$(QEMU_IMG):
	qemu-img create -f qcow2 $(QEMU_IMG) 1G

qemu: $(TARGET).iso
	qemu-system-i386 \
		-m 1G \
		-smp 2 \
		-boot d \
		-cdrom $(TARGET).iso \
		-display sdl,show-cursor=on \
		-serial mon:stdio

qemu_uefi: $(TARGET).iso
	qemu-system-x86_64 \
		-m 1G \
		-smp 2 \
		-boot d \
		-cdrom $(TARGET).iso \
		-display sdl,show-cursor=on \
		-serial mon:stdio \
		-smbios type=0,uefi=on -bios OVMF_BIOS.fd

clean:
	rm -rf $(OBJS) $(TARGET).elf $(TARGET).iso com1.out  *.s *.o *.out *.elf *.iso *.sym $(ISO_DIR) 
	rm -rf $(LIBCRANK_OBJS) $(LIBCRANK_TARGET)
	
.PHONY: clean bochs qemu
