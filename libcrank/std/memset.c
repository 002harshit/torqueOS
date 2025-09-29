#include "../std.h"

void *memset(void* s, int c, size_t n)
{
  unsigned char* a = s;
  for (; n; n--) {
    *a++ = c;
  }
  return s;
}

