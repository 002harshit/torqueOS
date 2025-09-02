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
#include "paging.h"
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
  lib_printf(" _> Key %c    Code %d\n", ascii, code);
}

void draw_torque_os_logo()
{
  fb_clear_screen();
  for (int i = 0; i < 5; i++) {
    lib_puts("\n");
  }
  const char* whitespace = "                  ";
  lib_printf("%s.-------------------------------------.\n", whitespace);
  lib_printf("%s|  _____ ___  ____   ___  _   _ _____  |\n", whitespace);
  lib_printf("%s| |_   _/ _ \\|  _ \\ / _ \\| | | | ____| |\n", whitespace);
  lib_printf("%s|   | || | | | |_) | | | | | | |  _|   |\n", whitespace);
  lib_printf("%s|   | || |_| |  _ <| |_| | |_| | |___  |\n", whitespace);
  lib_printf("%s|   | || |_| |  _ <| |_| | |_| | |___  |\n", whitespace);
  lib_printf("%s|   |_| \\___/|_| \\_\\\\__\\_\\\\___/|_____| |\n", whitespace);
  lib_printf("%s|              ___  ____               |\n", whitespace);
  lib_printf("%s|             / _ \\/ ___|              |\n", whitespace);
  lib_printf("%s|            | | | \\___ \\              |\n", whitespace);
  lib_printf("%s|            | |_| |___) |             |\n", whitespace);
  lib_printf("%s|             \\___/|____/              |\n", whitespace);
  lib_printf("%s.--------------------------------------.\n", whitespace);
}

void kmain()
{
  gdt_init();
  idt_init();
  paging_init();

  char *ptr = (char*)0xA0000000; // some random address
  char test_page_fault = *ptr;
  lib_printf("failed to do page fault: %c\n", test_page_fault);

  draw_torque_os_logo();
  lib_printf("\n\n");
  for (int i = 0; i < 80; i++) {
    lib_putchar('-');
  }
}
