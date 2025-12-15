#include <kernel/allocator.h>

#include <libcrank/std.h>

#include "./gfx.h"

#define DEBUG_COLOR (gfx_color_t) {.r = 255, .g = 0, .b = 255} /* magenta */

gfx_framebuffer_t _gfx = {0};

// INFO: under the hood the MM I/O linear framebuffer ie setup by BIOS which uses either GOP (in uefi) or VBE (in legacy bios)

void gfx_init(const struct multiboot_tag_framebuffer_common* mbi_fb)
{
  if (!mbi_fb) {
    printf("[WARN] Framebuffer tag provided is null\n");
    return;
  }
  if (mbi_fb->framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_RGB) {
    printf("[WARN] Failed to initialize gfx driver, got fb of type: %d\n", mbi_fb->framebuffer_type);
    return;
  }
  _gfx.width = mbi_fb->framebuffer_width;
  _gfx.height = mbi_fb->framebuffer_height;
  _gfx.pitch = mbi_fb->framebuffer_pitch;
  _gfx.bpp = mbi_fb->framebuffer_bpp / 8; // converting bits to bytes
  _gfx.fb = (unsigned char*) mbi_fb->framebuffer_addr;
  _gfx.buffer = allocator_malloc(sizeof(gfx_color_t) * _gfx.width * _gfx.height);
  
  printf("[INFO] Initialized gfx _> width: %d, height: %d, bpp: %d, pitch: %d\n", _gfx.width, _gfx.height, _gfx.bpp, _gfx.pitch);
  printf("[INFO] Sucessfully initialized gfx of type: %d\n", mbi_fb->framebuffer_type);
}

void gfx_setpixel(int x, int y, gfx_color_t color)
{
  if (x > -1 && y > -1 && x < _gfx.width && y < _gfx.height) {
    _gfx.buffer[y * _gfx.width + x] = color;
  }
}

gfx_color_t gfx_getpixel(int x, int y)
{
  if (x > -1 && y > -1 && x < _gfx.width && y < _gfx.height) {
    return _gfx.buffer[y * _gfx.width + x];
  }
  return DEBUG_COLOR; 
}

void gfx_clear(gfx_color_t color)
{
  for (int k = 0; k < _gfx.width * _gfx.height; k++) {
    _gfx.buffer[k] = color;
  }
}

void gfx_flush()
{
  for (int y = 0; y < _gfx.height; y++) {
    for (int x = 0; x < _gfx.width; x++) {
      gfx_color_t color = _gfx.buffer[y * _gfx.width + x];
      unsigned int offset = (y * _gfx.pitch) + (x * _gfx.bpp);

      // assuming the color format is BGRA
      _gfx.fb[offset + 0]  = color.b;
      _gfx.fb[offset + 1]  = color.g;
      _gfx.fb[offset + 2]  = color.r;
    }
  }
}

void gfx_close()
{
  allocator_free(_gfx.buffer); 
}
