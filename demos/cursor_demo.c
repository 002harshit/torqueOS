#include <driver/gfx.h>
#include <driver/mouse_ps2.h>
#include <driver/ps2.h>

#include <libcrank/std.h>
#include <libcrank/string.h>
#include <arch/x86/io.h>

static float prev_x = 255;
static float prev_y = 255;
static float cursor_x = 255;
static float cursor_y = 255;
static const float sensibility = 1;
static void cursor_cb(mouse_packet_t p)
{
  int x_delta = (int) p.x_movement;
  int y_delta = (int) p.y_movement;
  if (p.state.q.x_sign) {
    x_delta |= 0xFFFFFF00;
  }
  if (p.state.q.y_sign) {
    y_delta |= 0xFFFFFF00;
  }
  printf("DX: %d, DY: %d\n", x_delta, y_delta);;
  cursor_x += (float)x_delta * sensibility;
  cursor_y += (float)-y_delta * sensibility;
}

void cursor_demo()
{
  // BUG: when there are many interrupts, the main loop is blocked until there are no more interrupts
  // one reason it might produce alot of interrupts is that user moves mouse vigorously like a crazy dude
  // solution would be to either poll mouse events instead of interrupts or implement multithreading
  mouse_set_callback(cursor_cb);
  while (1) {
    __asm__ __volatile__("cli");
    gfx_color_t bg = {90, 250, 220};
    for (int j = 0; j < _gfx.height; j++) {
      for (int i = 0; i < _gfx.width; i++) {
        gfx_setpixel(i, j, bg);
      }
    }
    for (int k = 0; k < 10; k++) {
      prev_x = prev_x + (cursor_x - prev_x) * 0.1;
      prev_y = prev_y + (cursor_y - prev_y) * 0.1;

      gfx_color_t c = {200, 30, 70};

      for (int j = (int)prev_y; j < _gfx.height && j < (int)prev_y + 20; j++) {
        for (int i = (int)prev_x; i < _gfx.width && i < (int)prev_x + 20; i++) {
          if (i < 0 || j < 0 || i > _gfx.width || j > _gfx.height) continue;
          gfx_setpixel(i, j, c);
        }
      }
    }
    gfx_flush();
    __asm__ __volatile__("sti");
  }
}
