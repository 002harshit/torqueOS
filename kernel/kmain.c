#include "io.h"
#include "gdt.h"
#include "idt.h"
#include "scancode.h"
#include "common/lib.h"
#include "driver/framebuffer.h"

void lib_putchar(char c)
{
  fb_write(c);
}

void on_keyboard_interrupt()
{
  unsigned char code = inb(0x60);
  char ascii = scancode_to_ascii(code);
  if (ascii && code < 0x81)
    lib_printf("%c", scancode_to_ascii(code));
}

void kmain()
{
  gdt_init();
  idt_init();
  fb_clear_screen();
  lib_printf("Hello, Kernel");
}
