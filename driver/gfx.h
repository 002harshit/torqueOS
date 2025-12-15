#ifndef DRIVER_GFX_H
#define DRIVER_GFX_H

#include <kernel/multiboot2.h>

typedef struct {
  unsigned char r, g, b;
} gfx_color_t;

typedef struct {
  int width;
  int height;
  int pitch;
  int bpp;
  unsigned char* fb;
  gfx_color_t* buffer;
} gfx_framebuffer_t;

extern gfx_framebuffer_t _gfx;

// TODO: change interface for gfx: store mbi_fb related info in gfx_init
// and only allocate space for double-buffer in gfx_create

// must be called after retriving multiboot_info
void gfx_init(const struct multiboot_tag_framebuffer_common* mbi_fb);

// flushing the double buffer to the screen
void gfx_flush();

// operations for read/write to double buffer without changing the output
void gfx_setpixel(int x, int y, gfx_color_t color);
gfx_color_t gfx_getpixel(int x, int y);
void gfx_clear(gfx_color_t color);

void gfx_close();

#endif // DRIVER_GFX_H
