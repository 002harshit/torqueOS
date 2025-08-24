static const char ascii_keymap[256] = {
  0x0, 0x0, '1', '2', '3', '4', '5', '6',
  '7', '8', '9', '0', '-', '=', 0x0, '\t',
  'q','w', 'e', 'r', 't', 'y', 'u', 'i',
  'o', 'p', '[', ']', '\n', 0x0, 'a', 's',
  'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
  '\'', '`', 0x0, '\\', 'z', 'x', 'c', 'v',
  'b', 'n', 'm', ',', '.', '/', 0x0, '*', 0x0, ' '
};

static const char shift_ascii_keymap[256] = {
  0x0, 0x0, '!', '@', '#', '$', '%', '^',
  '&', '*', '(', ')', '_', '+', 0x0, '\t',
  'Q','W', 'E', 'R', 'T', 'Y', 'U', 'I',
  'O', 'P', '{', '}', '\n', 0x0, 'A', 'S',
  'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
  '\"', '`', 0x0, '|', 'Z', 'X', 'C', 'V',
  'B', 'N', 'M', '<', '>', '?', 0x0, '*', 0x0, ' '
};

#define RELEASED_OFFSET 0x80
#define SCANCODE_LSHIFT 0x2a

static int is_shift_down = 0;

char scancode_to_ascii(unsigned char code)
{
  if (code == SCANCODE_LSHIFT)
    is_shift_down = 1;
  if (code == SCANCODE_LSHIFT + RELEASED_OFFSET)
    is_shift_down = 0;

  if (code > RELEASED_OFFSET)
    code = code - RELEASED_OFFSET;

  if (is_shift_down)
    return shift_ascii_keymap[code];
  else
    return ascii_keymap[code];
}
