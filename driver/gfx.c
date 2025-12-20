#include <kernel/allocator.h>

#include <libcrank/std.h>

#include "./gfx.h"

#define DEBUG_COLOR (gfx_color_t) {.r = 255, .g = 0, .b = 255} /* magenta */

gfx_device_t _gfx = {0};

gfx_buffer_t gfx_buffer_create(int width, int height)
{
  gfx_buffer_t b;
  b.width = width;
  b.height = height;
  b.data = allocator_malloc(sizeof(gfx_color_t) * width * height);
  return b;
}

gfx_color_t gfx_buffer_getpixel(const gfx_buffer_t buf, int x, int y)
{
  if (x > -1 && y > -1 && x < buf.width && y < buf.height) {
    return buf.data[y * buf.width + x];
  }
  return DEBUG_COLOR; 
}

void gfx_buffer_setpixel(gfx_buffer_t buf, int x, int y, gfx_color_t color)
{
  if (x > -1 && y > -1 && x < buf.width && y < buf.height) {
    buf.data[y * buf.width + x] = color;
  }
}

void gfx_buffer_clear(gfx_buffer_t buf, gfx_color_t color)
{
  for (int k = 0; k < buf.width * buf.height; k++) {
    buf.data[k] = color;
  }
}

void gfx_buffer_blit(const gfx_buffer_t buf)
{
  int max_x = buf.width < _gfx.buffer.width ? buf.width : _gfx.buffer.width;
  int max_y = buf.height < _gfx.buffer.height ? buf.height : _gfx.buffer.height;
  for (int y = 0; y < max_y; y++) {
    for (int x = 0; x < max_x; x++) {
      _gfx.buffer.data[x + y * _gfx.buffer.width] = buf.data[x + y * buf.width];
    }
  }
}

void gfx_buffer_destroy(gfx_buffer_t* buf)
{
  allocator_free(buf->data);
  buf->data = NULL;
  buf->width = 0;
  buf->height = 0;
}

void gfx_init(const struct multiboot_tag_framebuffer_common* mbi_fb)
{
  // INFO: under the hood the MM I/O linear framebuffer ie setup by BIOS which uses either GOP (in uefi) or VBE (in legacy bios)
  if (!mbi_fb) {
    printf("[WARN] Framebuffer tag provided is null\n");
    return;
  }

  if (mbi_fb->framebuffer_bpp % 8 != 0) {
    printf("[WARN] Failed to initialize gfx, framebuffer_bpp (%d) must be divisible by 8\n", (int) mbi_fb->framebuffer_bpp);
    return;
  }

  if (mbi_fb->framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_RGB) {
    printf("[WARN] Failed to initialize gfx, invalid framebuffer_type (%d)\n", (int) mbi_fb->framebuffer_type);
    return;
  }

  _gfx.pitch = mbi_fb->framebuffer_pitch;
  _gfx.bpp = mbi_fb->framebuffer_bpp / 8; // converting bits to bytes
  _gfx.buffer = gfx_buffer_create(mbi_fb->framebuffer_width, mbi_fb->framebuffer_height);
  _gfx.fb = (unsigned char*) mbi_fb->framebuffer_addr;
  printf("[INFO] Sucessfully initialized gfx\n");
}

void gfx_flush()
{
  for (int y = 0; y < _gfx.buffer.height; y++) {
    for (int x = 0; x < _gfx.buffer.width; x++) {
      gfx_color_t color = _gfx.buffer.data[y * _gfx.buffer.width + x];
      unsigned int offset = (y * _gfx.pitch) + (x * _gfx.bpp);
      // assuming the color format is BGRA
      _gfx.fb[offset + 0]  = color.b;
      _gfx.fb[offset + 1]  = color.g;
      _gfx.fb[offset + 2]  = color.r;
    }
  }
}

void gfx_destroy()
{
  allocator_free(_gfx.buffer.data); 
}
