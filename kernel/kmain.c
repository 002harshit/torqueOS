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
#include "kheap.h"

#include "driver/framebuffer.h"
#include "driver/timer.h"
#include "driver/keyboard.h"

#include "libcrank/std.h"
#include "libcrank/string.h"

void putchar(char c)
{
  fb_write(c);
}

void timer_demo();

void malloc_demo()
{
  fb_clear_screen();
  gdt_init();
  idt_init();
  paging_init();

  char* buffer;
  // buffer = (char*)(0xDEAFBEEF); buffer[0] = 'a'; /* SHOULD PAGE FAULT */
  buffer = (char*)kmalloc(sizeof(char) * 64);

  strcpy(buffer, "Hello, Broo");
  printf("MSG: %s, ADDR: %x\n", buffer, (void*)buffer);
}

void kmain()
{
  timer_demo();
  return;
}

void draw_torque_os_logo()
{
  fb_clear_screen();
  fb_move(0);
  for (int i = 0; i < 5; i++) {
    puts("\n");
  }
  #define WHITESPACE "                  "
  puts(WHITESPACE ".-------------------------------------.\n");
  puts(WHITESPACE "|  _____ ___  ____   ___  _   _ _____  |\n");
  puts(WHITESPACE "| |_   _/ _ \\|  _ \\ / _ \\| | | | ____| |\n");
  puts(WHITESPACE "|   | || | | | |_) | | | | | | |  _|   |\n");
  puts(WHITESPACE "|   | || |_| |  _ <| |_| | |_| | |___  |\n");
  puts(WHITESPACE "|   | || |_| |  _ <| |_| | |_| | |___  |\n");
  puts(WHITESPACE "|   |_| \\___/|_| \\_\\\\__\\_\\\\___/|_____| |\n");
  puts(WHITESPACE "|              ___  ____               |\n");
  puts(WHITESPACE "|             / _ \\/ ___|              |\n");
  puts(WHITESPACE "|            | | | \\___ \\              |\n");
  puts(WHITESPACE "|            | |_| |___) |             |\n");
  puts(WHITESPACE "|             \\___/|____/              |\n");
  puts(WHITESPACE ".--------------------------------------.\n\n\n");
  puts(WHITESPACE "PRESS WSAD to Move");
  #undef WHITESPACE

}

int ticks = 0;
int is_falling = 0;
int pos = 19 * 80 + 38;
int prev = 19 * 80 + 38;
unsigned char last_keycode = 0;

void demo_update_callback()
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

  fb_move(0);
  printf("KEYCODE: %x  IS_RELEASED: %d  ", last_keycode, last_keycode > 0x80);
  printf("SHIFT: %d  CTRL: %d  ALT: %d\n", kb_is_key_pressed(0x2a), kb_is_key_pressed(0x1d), kb_is_key_pressed(0x38));
  printf("TICK: %d", timer_get_elapsed());
  
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

void demo_key_callback(unsigned char code, char is_released)
{
  fb_move(22 * 80 + 10);
  last_keycode = code;
  prev = pos;
  if (is_released) return;
  if (code == 0x1e) pos -= 1;
  if (code == 0x20) pos += 1;
  if (code == 0x11) pos -= 80;
  if (code == 0x1f) pos += 80;
}

/*
* round: 10-11th bits
* precision: 8-9th bits
*/
static inline void fpu_init(unsigned short round, unsigned short precision)
{
  unsigned short cw;
  __asm__ volatile ("fninit");
  __asm__ volatile ("fstcw %0" : "=m"(cw));

  cw &= ~0x0C00;
  cw |=  round;

  cw &= ~0x0300;
  cw |=  precision;

  __asm__ volatile ("fldcw %0" : : "m"(cw));
}

void timer_demo()
{
  gdt_init();
  idt_init();
  paging_init();

  fpu_init(0x0400, 0x0000);

  timer_stop();
  timer_set_callback(demo_update_callback);
  timer_start(62);

  draw_torque_os_logo();
  printf("\n");
  kb_init();
  kb_set_callback(demo_key_callback);
}
