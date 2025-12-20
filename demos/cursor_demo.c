#include <driver/gfx.h>
#include <driver/mouse_ps2.h>

#include <libcrank/std.h>
#include <libcrank/math.h>

volatile float prev_x = 255;
volatile float prev_y = 255;
volatile float cursor_x = 255;
volatile float cursor_y = 255;

// TODO: add keyboard controls to increase/decrease the mouse sensitivity
static float sensitivity = 1;

static void cursor_cb(mouse_packet_t p)
{
  // INFO: can be casted like this as signed data is stored in 2's complement
  // where sign bit is stored in MSB
  int x_delta = (signed char) p.x_movement;
  int y_delta = -(signed char) p.y_movement;
  printf("DX: %d, DY: %d\n", x_delta, y_delta);;
  cursor_x += (float)x_delta * sensitivity;
  cursor_y += (float)y_delta * sensitivity;
}

void cursor_demo()
{
  gfx_buffer_t gbuf = gfx_buffer_create(_gfx.buffer.width, _gfx.buffer.height);
  mouse_set_callback(cursor_cb);
  gfx_color_t c = {200, 30, 70};
  while (1) {
    gfx_color_t bg = {90, 250, 220};
    gfx_buffer_clear(gbuf, bg);
    prev_x += (cursor_x - prev_x) * 1;
    prev_y += (cursor_y - prev_y) * 1;
    for (float j = prev_y; j <  prev_y + 20; j++) {
      for (float i = prev_x; i < prev_x + 20; i++) {
        gfx_buffer_setpixel(gbuf, i, j, c);
      }
    }
    gfx_buffer_blit(gbuf);
    gfx_flush();
  }
  gfx_buffer_destroy(&gbuf);
}
