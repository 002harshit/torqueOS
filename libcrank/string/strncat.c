#include "../string.h"

char* strncat(char *restrict dst, const char *restrict src, size_t n)
{
  char* d = dst;
  for (; *d; d++) {}
  for (; n && *src; n--) {
    *d++ = *src++;
  }
  *d = '\0';
  return dst; 
}
