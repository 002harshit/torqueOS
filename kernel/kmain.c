/*
.--------------------------------------.
|  _____ ___  ____   ___  _   _ _____  |
| |_   _/ _ \|  _ \ / _ \| | | | ____| |
|   | || | | | |_) | | | | | | |  _|   |
|   | || |_| |  _ <| |_| | |_| | |___  |
|   |_| \___/|_| \_\\__\_\\___/|_____| |
|              ___  ____               |
|             / _ \/ ___|              |
|            | | | \___ \              |
|            | |_| |___) |             |
|             \___/|____/              |
|.-------------------------------------.
*/

#include <arch/x86/io.h>
#include <arch/x86/gdt.h>
#include <arch/x86/idt.h>
#include <arch/x86/paging.h>

#include <driver/gfx.h>
#include <driver/serial.h>
#include <driver/timer.h>
#include <driver/kb_ps2.h>
#include <driver/mouse_ps2.h>

#include <libcrank/std.h>
#include <libcrank/string.h>

#include "./kalloc.h"
#include "./multiboot2.h"
#include "./ramfs.h"
#include "./allocator.h"
#include "./mbi_info.h"

void putchar(char c)
{
  serial_write(c);
}

int is_protected_mode()
{
  unsigned int cr0_value;
  __asm__ __volatile__ (
    "mov eax, cr0\n"
    "mov %0, eax"
    : "=r" (cr0_value)
    :
    : "eax"
  );
  return cr0_value & 0x1;
}

void spinning_donut_demo();
void cursor_demo();
void ramfs_test();

extern unsigned int _kernel_end;

void kmain(unsigned int magic_number, multiboot_info_t* mbi)
{
  serial_init();

  if (magic_number != MULTIBOOT2_BOOTLOADER_MAGIC) {
    printf("[ERROR] Something is wrong with multiboot magic number: %x\nIt should be: %x\n", magic_number, MULTIBOOT2_BOOTLOADER_MAGIC);
    while(1) {}
    return;
}

  if (is_protected_mode()) {
    printf("[INFO] Kernel already in protected mode\n");
  } else {
    printf("[WARN] Kernel not running in protected mode\n");
  }

  gdt_init();
  paging_init();
  
  mbi_parsed_info_t mbi_info = parse_mbi(mbi);
  {
    mem_region_t used_regions[8];
    unsigned int used_regions_len = 0;
    used_regions[used_regions_len++] = (mem_region_t) {.start = 0, .end = (unsigned long long) &_kernel_end};
    if ((unsigned long long) mbi >= (unsigned long long) &_kernel_end) {
      used_regions[used_regions_len++] = (mem_region_t) {.start = (unsigned long long) mbi, .end = (unsigned long long) mbi + mbi->total_size};
    }
    unsigned long long fb_size = mbi_info.fb_tag->framebuffer_pitch * mbi_info.fb_tag->framebuffer_height;
    used_regions[used_regions_len++] = (mem_region_t) {.start = mbi_info.fb_tag->framebuffer_addr, .end = mbi_info.fb_tag->framebuffer_addr + fb_size};

    allocator_init(mbi_info.mmap_tag, used_regions, used_regions_len);
  }

  paging_enable();

  // Must be initialized before enabling interrupts
  // TODO: modify kb_init and mouse_init like so it can be initialized after initializing interrupts
  gfx_init(mbi_info.fb_tag);

  kb_init();
  mouse_init();

  idt_init();

  ramfs_test();

  timer_start(62);
  spinning_donut_demo();
  timer_stop();

  cursor_demo();

  // we don't want to get out of kmain so Kernel can listen to i/o events or interrupts
  while(1) {}
}
