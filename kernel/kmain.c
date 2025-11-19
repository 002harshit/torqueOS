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

#include <driver/vga.h>
#include <driver/serial.h>
#include <driver/timer.h>
#include <driver/kb_ps2.h>
#include <driver/mouse_ps2.h>

#include <libcrank/std.h>
#include <libcrank/string.h>

#include "./kalloc.h"
#include "./multiboot2.h"

void putchar(char c)
{
  serial_write(c);
}

void on_page_fault() {}

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

static float prev_x = 255;
static float prev_y = 255;
static float cursor_x = 255;
static float cursor_y = 255;
static const float sensibility = 1;
void cursor_cb(mouse_packet_t p)
{
  int x_delta = (int) p.x_movement;
  int y_delta = (int) p.y_movement;
  if (p.state.q.x_sign) {
    x_delta |= 0xFFFFFF00;
  }
  if (p.state.q.y_sign) {
    y_delta |= 0xFFFFFF00;
  }
  printf("DX: %d, DY: %d\n", x_delta, y_delta);;

  cursor_x += (float)x_delta * sensibility;
  cursor_y += (float)-y_delta * sensibility;
}

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

  printf("[INFO] MBI_SIZE: %d\n", mbi->total_size);

  // multiboot info contains array of tags ie terminated by tag.type = 0, tag.size = 8
  unsigned int tag_addr = (unsigned int) &mbi->tags;
  multiboot_tag_t* tag;
  do {
    tag = (multiboot_tag_t*)(tag_addr);
    switch (tag->type) {
      case MULTIBOOT_TAG_TYPE_CMDLINE: {
        struct multiboot_tag_string* t = (void*) tag;
        printf("[INFO] CMDLINE ARGS: %s\n", t->string);
      } break;

      case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME: {
        struct multiboot_tag_string* t = (void*) tag;
        printf("[INFO] BOOT_LOADER_NAME: %s\n", t->string);
      } break;

      case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
        struct multiboot_tag_framebuffer_common* t = (void*) tag;
        vga_init(t);
      } break;

      default: {
        printf("[WARN] MBI type {%d} not handled by parser\n", tag->type);
      }
    }
    tag_addr += MULTIBOOT_ALIGN_TAG(tag->size);
  } while (!(tag->type == 0 && tag->size == 8));

  unsigned int mbi_size = (unsigned int) tag_addr - (unsigned int)mbi;
  if (mbi_size != mbi->total_size) {
    printf("[ERROR] MBI SIZE CHECK FAILED: Expected <%d>  Got: <%d>\n", mbi->total_size, mbi_size);
    while(1) {}
  }

  // Must be initialized before enabling interrupts
  // TODO: modify kb_init and mouse_init like so it can be initialized after initializing interrupts
  kb_init();
  mouse_init();
  mouse_set_callback(cursor_cb);

  idt_init();

  // BUG: Right now the cursor shows visual artifacts when compiled with optimization higher than -O1
  // there is probably a bug in vga_setpixel or vga_flush
  // i should draw pixel using stride = pitch / bytes_per_pixel instead of width
  while (1) {
    vga_color_t bg = {90, 250, 220};
    for (int j = 0; j < vga.height; j++) {
      for (int i = 0; i < vga.width; i++) {
        vga_setpixel(i, j, bg);
      }
    }

    prev_x = prev_x + (cursor_x - prev_x) * 0.4;
    prev_y = prev_y + (cursor_y - prev_y) * 0.4;
    vga_color_t c = {200, 30, 70};
    for (int j = (int)prev_y; j < vga.height && j < (int)prev_y + 20; j++) {
      for (int i = (int)prev_x; i < vga.width && i < (int)prev_x + 20; i++) {
        if (i < 0 || j < 0 || i > vga.width || j > vga.height) continue;
        vga_setpixel(i, j, c);
      }
    }

    vga_flush();
  }


  // BUG: PIC stops sending keyboard interrupts after timer_start
  // This only occurs when timer's interrupt handler does a computation heavy task
  // which blocks other interrupts with less priority
  // timer_start(62);

  // we don't want to get out of kmain so Kernel can listen to i/o events or interrupts
  while(1) {}
}
