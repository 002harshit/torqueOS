TOOLCHAIN ?=$(HOME)/opt/cross_i386-elf/bin/i386-elf-
CC=$(TOOLCHAIN)gcc
AR=$(TOOLCHAIN)ar
LD=$(TOOLCHAIN)ld

TARGET=torque

C_SOURCES =
C_SOURCES += $(wildcard arch/x86/*.c)
C_SOURCES += $(wildcard driver/*.c)
C_SOURCES += $(wildcard kernel/*.c)
C_SOURCES += demos/spinning_donut.c demos/cursor.c

ASM_SOURCES =
ASM_SOURCES += $(wildcard kernel/*.asm)
ASM_SOURCES += $(wildcard arch/x86/*.asm)

HEADERS =
HEADERS += $(wildcard libcrank/*.h)
HEADERS += $(wildcard arch/x86/*.h)
HEADERS += $(wildcard kernel/*.h)

OBJS =
OBJS += $(C_SOURCES:.c=.o)
OBJS += $(ASM_SOURCES:.asm=.s.o)

LIBCRANK_TARGET=libcrank.a

LIBCRANK_SOURCES = 
LIBCRANK_SOURCES += $(wildcard libcrank/std/*.c)
LIBCRANK_SOURCES += $(wildcard libcrank/string/*.c)
LIBCRANK_SOURCES += $(wildcard libcrank/math/*.c)

LIBCRANK_OBJS += $(LIBCRANK_SOURCES:.c=.o)

LIBCRANK_HEADERS = $(wildcard libcrank/*.h)

CFLAGS ?=
CFLAGS += -std=gnu99 -ffreestanding -nostdlib -O3 -Wall -Wextra -g -ggdb -m32
CFLAGS += -I./
CFLAGS += -masm=intel

QEMU_IMG = qemu_disk.qcow2

ISO_DIR = iso

# for ikos static analysing
LLVM_BC =$(C_SOURCES:.c=.bc)
LLVM_BC +=$(LIBCRANK_SOURCES:.c=.bc)
CLANG_IKOS_FLAGS = $(CFLAGS)
CLANG_IKOS_FLAGS += -D_FORTIFY_SOURCE=0 -D__IKOS__ -g -O0 -Xclang -disable-O0-optnone
CLANG_IKOS_FLAGS += -target i386-elf -march=i386
IKOS_REPORT = report.db
LLVM_BC_OUTPUT = torque.bc

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
	rm -rf $(LLVM_BC) $(IKOS_REPORT) $(LLVM_BC_OUTPUT)
	
# for my setup /dev/sda is path to my pendrive
FLASH_PATH ?=
ifeq ($(FLASH_PATH), )
flash:
	@echo "[ERROR] execute this command like so: FLASH_PATH=/dev/sda make flash"
else
flash:
	sudo dd if=$(TARGET).iso of=$(FLASH_PATH) bs=4M status=progress oflag=sync
endif

%.bc: %.c $(HEADERS) $(LIBCRANK_HEADERS)
	clang-14  $(CLANG_IKOS_FLAGS) -emit-llvm -c $< -o $@

$(LLVM_BC_OUTPUT): $(LLVM_BC)
	llvm-link-14 $(LLVM_BC) -o $(LLVM_BC_OUTPUT)

$(IKOS_REPORT): $(LLVM_BC_OUTPUT)
	ikos $(LLVM_BC_OUTPUT) --entry-points=kmain --no-libc -o $(IKOS_REPORT)

ikos: $(IKOS_REPORT)
	ikos-view $(IKOS_REPORT)

.PHONY: clean bochs qemu flash ikos
