#include "../string.h"

char* strncpy(char* restrict dst, const char* restrict src, size_t n)
{
  char* d = dst;
  for (; n && *src; src++, n--) {
    *d++ = *src;
  }
  *d = '\0';
  return dst;
}
