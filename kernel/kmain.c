#include "gdt.h"
#include "common/lib.h"
#include "driver/framebuffer.h"

void lib_putchar(char c)
{
  fb_write(c);
}

void kmain()
{
  gdt_init();
  fb_clear_screen();
  lib_printf("Hello, Kernel");
}
