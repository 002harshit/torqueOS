#include <driver/vga.h>
#include <driver/mouse_ps2.h>

#include <libcrank/std.h>
#include <libcrank/string.h>

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
  mouse_set_callback(cursor_cb);
  while (1) {
    vga_color_t bg = {90, 250, 220};
    for (int j = 0; j < vga.height; j++) {
      for (int i = 0; i < vga.width; i++) {
        vga_setpixel(i, j, bg);
      }
    }
    prev_x = prev_x + (cursor_x - prev_x) * 0.4;
    prev_y = prev_y + (cursor_y - prev_y) * 0.4;
    vga_color_t c = {200, 30, 70};
    for (int j = (int)prev_y; j < vga.height && j < (int)prev_y + 20; j++) {
      for (int i = (int)prev_x; i < vga.width && i < (int)prev_x + 20; i++) {
        if (i < 0 || j < 0 || i > vga.width || j > vga.height) continue;
        vga_setpixel(i, j, c);
      }
    }
    vga_flush();
  }
}
