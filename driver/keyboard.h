#ifndef DRIVER_KEYBOARD_H
#define DRIVER_KEYBOARD_H

// TODO: create enums for every scancode (which is quite easy but im lazy)
#define SCANCODE_LSHIFT 0x2a
#define SCANCODE_LCTRL 0x1d

typedef void(*KeyboardCallback)(unsigned char keycode, char is_released);

void kb_init();
char kb_is_key_pressed(unsigned char keycode);
void kb_set_callback(KeyboardCallback handler);

#endif // DRIVER_KEYBOARD_H
