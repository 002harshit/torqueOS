#include "../string.h"

char* strncpy(char* restrict dst, const char* restrict src, size_t n)
{
  char* d = dst;
  for (; n && *src; src++, n--) {
    *d++ = *src;
  }
  // pad with null if n > len(src)
  if (n) {
    for (; n; n--) {
      *d++ = '\0';
    }
  }
  *d = '\0';
  return dst;
}
