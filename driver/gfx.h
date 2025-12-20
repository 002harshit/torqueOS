#ifndef DRIVER_GFX_H
#define DRIVER_GFX_H

#include <kernel/multiboot2.h>

typedef struct {
  unsigned char r, g, b;
} gfx_color_t;

typedef struct {
  int width;
  int height;
  gfx_color_t* data;
} gfx_buffer_t;

typedef struct {
  int pitch;
  int bpp;
  gfx_buffer_t buffer;
  unsigned char* fb;
} gfx_device_t;

extern gfx_device_t _gfx;

// gfx_init must be called after initializing allocator
void gfx_init(const struct multiboot_tag_framebuffer_common* mbi_fb);
void gfx_flush();
void gfx_destroy();

gfx_buffer_t gfx_buffer_create(int width, int height);
gfx_color_t gfx_buffer_getpixel(const gfx_buffer_t buf, int x, int y);
void gfx_buffer_setpixel(gfx_buffer_t buf, int x, int y, gfx_color_t color);
void gfx_buffer_clear(gfx_buffer_t buf, gfx_color_t color);
void gfx_buffer_blit(const gfx_buffer_t buf);
void gfx_buffer_destroy(gfx_buffer_t* buf);

#endif // DRIVER_GFX_H
