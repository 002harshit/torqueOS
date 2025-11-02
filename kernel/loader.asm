global kernel_loader

KERNEL_STACK_SIZE equ 4096

MAGIC_NUMBER equ 0xE85250D6 ; for multiboot2
ARCH equ 0x0 ; x86
FLAGS equ 0x0

section .multiboot_header
align 8 ; multiboot header must be aligned 8bytes, doing it in linker instead will work as well
mb_header:
  dd MAGIC_NUMBER
  dd ARCH
  dd mb_header_end - mb_header ; HEADER LENGTH
  dd -(MAGIC_NUMBER + ARCH + (mb_header_end - mb_header)) ; CHECK_SUM
mb_header_end:

section .text
extern kmain

kernel_loader:
  ; TODO: place a 'guard page' before the kernel stack, so it page faults upon stack overflow
  mov esp, kernel_stack + KERNEL_STACK_SIZE
  ;xchg bx, bx
  push ebx ; ebx contains physical pointer to multiboot
  push eax ; and eax contains MAGIC NUMBER
  call kmain

.loop:
    jmp .loop

section .bss
align 4
kernel_stack:
	resb KERNEL_STACK_SIZE
