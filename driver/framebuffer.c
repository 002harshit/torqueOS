#include "../io.h"

#define COMMAND_PORT 0x3D4
#define DATA_PORT 0x3D5

#define HIGH_BYTE_COMMAND 14
#define LOW_BYTE_COMMAND 15

#define FB_WIDTH 80
#define FB_HEIGHT 25
#define FB_SIZE FB_WIDTH * FB_HEIGHT

static char* fb = (char*) 0x000B8000;
static unsigned short cursor_pos = 0;

void fb_move(unsigned short pos)
{
  outb(COMMAND_PORT, HIGH_BYTE_COMMAND);
  outb(DATA_PORT, ((pos >> 8) & 0x00FF));

  outb(COMMAND_PORT, LOW_BYTE_COMMAND);
  outb(DATA_PORT, pos & 0x00FF);
}

void fb_write_cell(unsigned int i, char c, unsigned short color)
{
  fb[i] = c;
  fb[i + 1] = color;
}

void fb_clear_screen()
{
  for (int i = 0; i < 80 * 25; i++) {
    fb_write_cell(i*2, ' ', 0x00);
  }
}

void fb_scroll()
{
  // scrolling one line up
  for (int i = FB_WIDTH; i < FB_SIZE; i++) {
    int j = i - FB_WIDTH;
    fb[j * 2] = fb[i * 2];
    fb[j * 2 + 1] = fb[i * 2 + 1];
  }

  // clearing last line
  for (int c = 0; c < FB_WIDTH; c++) {
    int k = (24) * FB_WIDTH + c;
    fb[k * 2] = 0;
    fb[k * 2 + 1] = 0;
  }
}

void fb_write(char c)
{
	if (c == '\n') {
	cursor_pos = (cursor_pos / 80 + 1) * 80;
	  while (cursor_pos >= FB_SIZE) {
	    fb_scroll();
	    cursor_pos -= FB_WIDTH;
	  }
	} else {
	fb_write_cell(cursor_pos * 2, c, 0x0F);
	  cursor_pos++;
	  if (cursor_pos == FB_SIZE) {
	    fb_scroll();
	    cursor_pos -= FB_WIDTH;
	  }
	}
	fb_move(cursor_pos);
}
