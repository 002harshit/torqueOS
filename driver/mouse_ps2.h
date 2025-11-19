#ifndef DRIVER_MOUSE_PS2_H
#define DRIVER_MOUSE_PS2_H

typedef struct {
  union {
    struct {
      unsigned char left_button:   1;
      unsigned char right_button:  1;
      unsigned char middle_button: 1;
      unsigned char _unused:       1;
      unsigned char x_sign:        1;
      unsigned char y_sign:        1;
      unsigned char x_overflow:    1;
      unsigned char y_overflow:    1;
    } q;
    unsigned char raw;
  } state;
  unsigned char x_movement;
  unsigned char y_movement;
} mouse_packet_t;

_Static_assert(sizeof(mouse_packet_t) == 3, "mouse_packet_t must be of size 3 bytes"); 

typedef void(*MouseCallback)(mouse_packet_t);

void mouse_init();
void mouse_set_callback(MouseCallback handler);

#endif // DRIVER_MOUSE_PS2_H
