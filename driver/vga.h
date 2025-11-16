#ifndef DRIVER_VGA_FRAMEBUFFER_H
#define DRIVER_VGA_FRAMEBUFFER_H

#include <kernel/multiboot2.h>

typedef struct {
  unsigned char r, g, b;
} vga_color_t;

typedef struct {
  int width;
  int height;
  int pitch;
  int bytes_per_pixel;
  unsigned char* fb;
  vga_color_t* buffer;
} vga_framebuffer_t;

extern vga_framebuffer_t vga;

// must be called after retriving multiboot_info
void vga_init(const struct multiboot_tag_framebuffer_common* mbi_fb);

// flushing the double buffer to the screen
void vga_flush();

// operations for read/write to double buffer without changing the output
void vga_setpixel(int x, int y, vga_color_t color);
vga_color_t vga_getpixel(int x, int y);
void vga_clear(vga_color_t color);

#endif // DRIVER_VGA_FRAMEBUFFER_H
