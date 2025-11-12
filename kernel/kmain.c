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

#include "io.h"
#include "gdt.h"
#include "idt.h"
#include "kalloc.h"
#include "multiboot2.h"

#include "driver/vga.h"
#include "driver/serial.h"
#include "driver/timer.h"
#include "driver/keyboard.h"

#include "libcrank/std.h"
#include "libcrank/string.h"

__attribute__((section(".multiboot_header")))
const struct multiboot_header_tag_framebuffer framebuffer_tag = {
    .type = MULTIBOOT_HEADER_TAG_FRAMEBUFFER,
    .size = sizeof(framebuffer_tag),
    .flags = 0,
    // width, height, depth not set so bios will choose the default framebuffer
};

void putchar(char c)
{
  serial_write(c);
}

void on_page_fault() {}

int keyboard_data_pending(void) {
    return inb(0x64) & 0x01;
}

void spinning_donut_demo();
void kmain(unsigned int magic_number, multiboot_info_t* mbi)
{
  serial_init();
  gdt_init();
  idt_init();
  if (magic_number != MULTIBOOT2_BOOTLOADER_MAGIC) {
    printf("[ERROR] Something is wrong with multiboot magic number: %x\nIt should be: %x\n", magic_number, MULTIBOOT2_BOOTLOADER_MAGIC);
    while(1) {}
    return;
  }

  printf("[INFO] MBI_SIZE: %d\n", mbi->total_size);

  // multiboot info contains array of tags ie terminated by tag.type = 0, tag.size = 8
  unsigned int tag_addr = (unsigned int) &mbi->tags;
  multiboot_tag_t* tag;
  do {
    tag = (multiboot_tag_t*)(tag_addr);
    switch (tag->type) {
      case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR: {
      } break;

      case MULTIBOOT_TAG_TYPE_CMDLINE: {
        struct multiboot_tag_string* t = (void*) tag;
        printf("> CMDLINE: %s\n", t->string);
      } break;

      case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME: {
        struct multiboot_tag_string* t = (void*) tag;
        printf("> BOOT_LOADER_NAME: %s\n", t->string);
      } break;

      case MULTIBOOT_TAG_TYPE_APM: {
      } break;

      case MULTIBOOT_TAG_TYPE_MMAP: {
      } break;

      case MULTIBOOT_TAG_TYPE_ELF_SECTIONS: {
      } break;

      case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO: {
      } break;

      case MULTIBOOT_TAG_TYPE_BOOTDEV: {
      } break;

      case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
        struct multiboot_tag_framebuffer_common* t = (void*) tag;
        vga_init(t);
      } break;

      case MULTIBOOT_TAG_TYPE_ACPI_OLD: {
      } break;

      case MULTIBOOT_TAG_TYPE_END: {
      } break;

      default: {
        printf("[WARN] MBI type <%d> not handled by parser\n", tag->type);
      }
    }
    tag_addr += MULTIBOOT_ALIGN_TAG(tag->size);
  } while (!(tag->type == 0 && tag->size == 8));

  unsigned int mbi_size = (unsigned int) tag_addr - (unsigned int)mbi;
  if (mbi_size != mbi->total_size) {
    printf("[ERROR] MBI SIZE CHECK FAILED: Expected <%d>  Got: <%d>\n", mbi->total_size, mbi_size);
    while(1) {}
  }

  // BUG: PIC stops sending keyboard interrupts after timer_start
  // This only occurs when timer's interrupt handler does a computation heavy task
  // which blocks other interrupts with less priority
  // timer_start(62);

  kb_init();
  spinning_donut_demo(); 

  // we don't want to get out of kmain so Kernel can listen to i/o events or interrupts
  while(1) {}
}
