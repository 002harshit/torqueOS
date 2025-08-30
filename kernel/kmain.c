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

volatile int pos = 0;
void change_dir(unsigned char code)
{
  if (code == 0x1E) {
    pos -= 1;
  } else if (code == 0x20) {
    pos += 1;
  } else if (code == 0x1F) {
    pos += 80;
  } else if (code == 0x11) {
    pos -= 80;
  }
}

void on_keyboard_interrupt()
{
  unsigned char code = inb(0x60);
  change_dir(code);
}

char my_fb[80 * 25 * 2];

void write_cell(int i, char c, unsigned short color)
{
  i %= 80*25;
  if (i < 0) {
    i = 80 * 25 + i;
  }
  i*=2;

  my_fb[i] = c;
  my_fb[i + 1] = color;
}

void write(int i, char* buf) {
  for (int j = 0; buf[j]; j++) {
    write_cell(i + j, buf[j], 0x0F);
  }
}

void clear_screen()
{
  for (int i = 0; i < 80 * 25; i++) {
    write_cell(i, ' ', 0x0);
  }
}

void blit_screen()
{
  for (int i = 0; i < 80 * 25; i++) {
    fb_write_cell(i * 2, my_fb[i * 2], (unsigned char)my_fb[i * 2 + 1]);
  }
}

void kmain()
{
  gdt_init();
  idt_init();
  fb_clear_screen();
  while(1) {
    clear_screen();
    write(pos, "WSAD to Move");
    blit_screen();
  }
}
