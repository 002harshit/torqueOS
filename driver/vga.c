#include "vga.h"
#include "kernel/kalloc.h"
#include "libcrank/std.h"

#define DEBUG_COLOR (vga_color_t) {.r = 1, .g = 0, .b = 1} /* magenta */

vga_framebuffer_t vga = {0};

void vga_init(const struct multiboot_tag_framebuffer_common* mbi_fb)
{
  // right now only rgb framebuffer is implemented
  if (mbi_fb->framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_RGB) {
    return;
  }
  vga.width = mbi_fb->framebuffer_width;
  vga.height = mbi_fb->framebuffer_height;
  vga.pitch = mbi_fb->framebuffer_pitch;
  vga.bytes_per_pixel = mbi_fb->framebuffer_bpp / 8; // converting bits to bytes
  vga.fb = (unsigned char*) mbi_fb->framebuffer_addr;
  vga.buffer = (void*) kalloc(sizeof(vga_color_t) * vga.width * vga.height, 0);
  
  printf("[INFO] Sucessfully initialized vga of type: %d\n", mbi_fb->framebuffer_type);
}

void vga_setpixel(int x, int y, vga_color_t color)
{
  if (x > -1 && y > -1 && x < vga.width && y < vga.height) {
    vga.buffer[y * vga.width + x] = color;
  }
}

vga_color_t vga_getpixel(int x, int y)
{
  if (x > -1 && y > -1 && x < vga.width && y < vga.height) {
    return vga.buffer[y * vga.width + x];
  }
  return DEBUG_COLOR; 
}

void vga_clear(vga_color_t color)
{
  for (int k = 0; k < vga.width * vga.height; k++) {
    vga.buffer[k] = color;
  }
}

void vga_flush()
{
  for (int y = 0; y < vga.height; y++) {
    for (int x = 0; x < vga.width; x++) {
      vga_color_t color = vga.buffer[y * vga.width + x];
      unsigned int offset = (y * vga.pitch) + (x * vga.bytes_per_pixel);

      // assuming the color format is BGRA
      vga.fb[offset + 0]  = color.b;
      vga.fb[offset + 1]  = color.g;
      vga.fb[offset + 2]  = color.r;
    }
  }
}
