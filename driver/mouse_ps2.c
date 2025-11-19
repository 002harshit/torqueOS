#include <arch/x86/io.h>

#include <libcrank/std.h>

#include "./ps2.h"
#include "./mouse_ps2.h"

void mouse_init()
{
  ps2_outb(PS2_COMMAND_PORT, PS2_ENABLE_MOUSE);

  ps2_outb(PS2_COMMAND_PORT, PS2_CONFIG_READ);
  unsigned char config = ps2_inb(PS2_DATA_PORT) | 0x02; // enable irq for ps/2 mouse

  ps2_outb(PS2_COMMAND_PORT, PS2_CONFIG_WRITE);
  ps2_outb(PS2_DATA_PORT, config);

  unsigned char ack;
  ps2_outb(PS2_COMMAND_PORT, PS2_MOUSE_PASSTHROUGH);
  ps2_outb(PS2_DATA_PORT, PS2_RESETS_NO_TEST);
  ack = ps2_inb(PS2_DATA_PORT);

  ps2_outb(PS2_COMMAND_PORT, PS2_MOUSE_PASSTHROUGH);
  ps2_outb(PS2_DATA_PORT, PS2_ENABLE_REPORTING);
  ack = ps2_inb(PS2_DATA_PORT);

  printf("[INFO] Initialized Onboard Mouse Driver\n");
}

static void _default_handler(mouse_packet_t p)
{
  (void) p;
}

static MouseCallback mouse_handler = _default_handler;
void mouse_set_callback(MouseCallback handler)
{
  mouse_handler = handler;
}

static int mouse_packet_no = -1;
static mouse_packet_t packet = {0};
void on_mouse_interrupt()
{
  // TODO: handle and init mouse device type: 4 (for scrolling wheel support)
  mouse_packet_no++;
  mouse_packet_no = mouse_packet_no % 3;
  unsigned char data = inb(PS2_DATA_PORT);
  switch (mouse_packet_no) {
    case 0: {
      packet.state.raw = data;
    } break;
    case 1: {
      packet.x_movement = data;
    } break;
    case 2: {
      packet.y_movement = data;
      // haha too big printf
      printf("MOUSE:\n\tleft (%d), right (%d), mid (%d)\n\txs: (%d), ys: (%d)\n\txo: (%d), yo: (%d)\n\txm: (%d), ym: (%d)\n", packet.state.q.left_button, packet.state.q.right_button, packet.state.q.middle_button, packet.state.q.x_sign, packet.state.q.y_sign, packet.state.q.x_overflow, packet.state.q.y_overflow, packet.x_movement, packet.y_movement);
      mouse_handler(packet);
    } break;
  }
}
