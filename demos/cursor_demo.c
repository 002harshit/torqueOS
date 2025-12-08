#include <driver/gfx.h>
#include <driver/mouse_ps2.h>

#include <libcrank/std.h>
#include <libcrank/math.h>

volatile float prev_x = 255;
volatile float prev_y = 255;
volatile float cursor_x = 255;
volatile float cursor_y = 255;
static void cursor_cb(mouse_packet_t p)
{
  // INFO: can be casted like this as signed data is stored in 2's complement
  // where sign bit is stored in MSB
  int x_delta = (signed char) p.x_movement;
  int y_delta = -(signed char) p.y_movement;
  printf("DX: %d, DY: %d\n", x_delta, y_delta);;
  cursor_x += (float)x_delta;
  cursor_y += (float)y_delta;
}

void cursor_demo()
{
  mouse_set_callback(cursor_cb);
  gfx_color_t c = {200, 30, 70};
  while (1) {
    gfx_color_t bg = {90, 250, 220};
    for (int j = 0; j < _gfx.height; j++) {
      for (int i = 0; i < _gfx.width; i++) {
        gfx_setpixel(i, j, bg);
      }
    }

    prev_x += (cursor_x - prev_x) * 0.1;
    prev_y += (cursor_y - prev_y) * 0.1;
    for (int j = (int)prev_y; j < _gfx.height && j < (int)prev_y + 20; j++) {
      for (int i = (int)prev_x; i < _gfx.width && i < (int)prev_x + 20; i++) {
        if (i < 0 || j < 0 || i >= _gfx.width || j >= _gfx.height) continue;
        gfx_setpixel(i, j, c);
      }
    }
    gfx_flush();
 }
}
