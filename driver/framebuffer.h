#ifndef DRIVER_FRAMEBUFFER_H
#define DRIVER_FRAMEBUFFER_H

#warning "driver/framebuffer.h is deprecated, use driver/vga.h instead"

void fb_move(unsigned short pos);
void fb_write_cell(unsigned int i, char c, unsigned short color);
void fb_clear_screen();
void fb_write(char c);
unsigned short fb_cursor();

#endif // DRIVER_FRAMEBUFFER_H
