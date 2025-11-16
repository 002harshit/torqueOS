#include <arch/x86/io.h>

#include <libcrank/std.h>

#include "./kb_ps2.h"

#define RELEASED_OFFSET 0x80
#define IS_RELEASED(c) (c > RELEASED_OFFSET)

static char kb_pressed[RELEASED_OFFSET] = {0};

static void _default_handler(unsigned char keycode, char is_released)
{
  (void) keycode;
  (void) is_released;
}

static KeyboardCallback kb_handler = _default_handler;

// static 
const char ascii_keymap[256] = {
  0x0, 0x0, '1', '2', '3', '4', '5', '6',
  '7', '8', '9', '0', '-', '=', 0x0, '\t',
  'q','w', 'e', 'r', 't', 'y', 'u', 'i',
  'o', 'p', '[', ']', '\n', 0x0, 'a', 's',
  'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
  '\'', '`', 0x0, '\\', 'z', 'x', 'c', 'v',
  'b', 'n', 'm', ',', '.', '/', 0x0, '*', 0x0, ' '
};

// static 
const char shift_ascii_keymap[256] = {
  0x0, 0x0, '!', '@', '#', '$', '%', '^',
  '&', '*', '(', ')', '_', '+', 0x0, '\t',
  'Q','W', 'E', 'R', 'T', 'Y', 'U', 'I',
  'O', 'P', '{', '}', '\n', 0x0, 'A', 'S',
  'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
  '\"', '`', 0x0, '|', 'Z', 'X', 'C', 'V',
  'B', 'N', 'M', '<', '>', '?', 0x0, '*', 0x0, ' '
};

static unsigned char read_keycode()
{
  unsigned char code = inb(0x60);
  if (IS_RELEASED(code)) {
    kb_pressed[code - RELEASED_OFFSET] = 0;
  } else {
    kb_pressed[code] = 1;
  }
  return code;
}

char kb_to_ascii(unsigned char code)
{
  if (IS_RELEASED(code))
    code = code - RELEASED_OFFSET;

  if (kb_pressed[SCANCODE_LSHIFT])
    return shift_ascii_keymap[code];
  else
    return ascii_keymap[code];
}

char kb_is_key_pressed(unsigned char code)
{
  if (code > RELEASED_OFFSET) return 0;
  return kb_pressed[code];
}

void kb_init()
{
  // Reference: https://github.com/AlgorithMan-de/wyoos/blob/master/src/drivers/keyboard.cpp
  while (inb(0x64) & 0x1) {
    char _ = inb(0x60); // read key
  }
  outb(0x64, 0xae); // start interrupts
  outb(0x64, 0x20); // ask to read keyboard controller command byte
  unsigned char status = (inb(0x60)  | 1) & ~0x10; // clear bit-4
  outb(0x64, 0x60); // set controller command byte
  outb(0x60, status);
  outb(0x60, 0xf4);

  printf("[INFO] Initialized Onboard Keyboard Driver\n");
}

void kb_set_callback(KeyboardCallback handler)
{
  kb_handler = handler;
}

void on_keyboard_interrupt()
{
  unsigned char code = read_keycode();
  kb_handler(code, IS_RELEASED(code));
}

