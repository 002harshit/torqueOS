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

void draw_torque_os_logo()
{
  fb_clear_screen();
  fb_move(0);
  for (int i = 0; i < 5; i++) {
    lib_puts("\n");
  }
  #define WHITESPACE "                  "
  lib_puts(WHITESPACE ".-------------------------------------.\n");
  lib_puts(WHITESPACE "|  _____ ___  ____   ___  _   _ _____  |\n");
  lib_puts(WHITESPACE "| |_   _/ _ \\|  _ \\ / _ \\| | | | ____| |\n");
  lib_puts(WHITESPACE "|   | || | | | |_) | | | | | | |  _|   |\n");
  lib_puts(WHITESPACE "|   | || |_| |  _ <| |_| | |_| | |___  |\n");
  lib_puts(WHITESPACE "|   | || |_| |  _ <| |_| | |_| | |___  |\n");
  lib_puts(WHITESPACE "|   |_| \\___/|_| \\_\\\\__\\_\\\\___/|_____| |\n");
  lib_puts(WHITESPACE "|              ___  ____               |\n");
  lib_puts(WHITESPACE "|             / _ \\/ ___|              |\n");
  lib_puts(WHITESPACE "|            | | | \\___ \\              |\n");
  lib_puts(WHITESPACE "|            | |_| |___) |             |\n");
  lib_puts(WHITESPACE "|             \\___/|____/              |\n");
  lib_puts(WHITESPACE ".--------------------------------------.\n\n\n");
  lib_puts(WHITESPACE "PRESS WSAD to Move");
  #undef WHITESPACE

}

int ticks = 0;
int is_falling = 0;
int pos = 19 * 80 + 38;
int prev = 19 * 80 + 38;
void on_timer_interrupt()
{
  static const char player_fall[] = {
  ' ', 'o', ' ',
  '/', '|', '\\',
  '/', ' ', '\\'
  };

  static const char player_jump[] = {
  '\\', 'o', '/',
  ' ',  '|', ' ',
  '/',  ' ', '\\'
  };
  
  if (ticks++ % 16 == 0) {
    draw_torque_os_logo();
    is_falling = !is_falling;
  }
  
  const char* sprite = player_jump;
  if (is_falling) sprite = player_fall;

  fb_move(prev);
  for (int k = 0; k < 9; k++) {
    int r = k / 3;
    int c = k % 3;
    fb_move(prev + c + r * 80);
    fb_write(' ');
  }

  fb_move(pos);
  for (int k = 0; k < 9; k++) {
    int r = k / 3;
    int c = k % 3;
    fb_move(pos + c + r * 80);
    fb_write(sprite[k]);
  }

}
void on_keyboard_interrupt()
{
  prev = pos;
  unsigned char code = inb(0x60);
  if (code == 0x1e) pos -= 1;
  if (code == 0x20) pos += 1;
  if (code == 0x11) pos -= 80;
  if (code == 0x1f) pos += 80;
}

void timer_init(int frequency)
{
  unsigned int divisor = 1193180 / frequency;
  unsigned char l = divisor & 0xFF;
  unsigned char r = (divisor >> 8) & 0xFF;

  outb(0x43, 0x36);
  outb(0x40, l);
  outb(0x40, r);
}

void kmain()
{
  gdt_init();
  idt_init();
  paging_init();
  timer_init(62);
}
