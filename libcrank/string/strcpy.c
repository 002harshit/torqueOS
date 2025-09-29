#include "../string.h"

char* strcpy(char* restrict dst, const char* restrict src)
{
  char* d = dst;
  for (; *src; src++) {
    *d++ = *src;
  }
  *d = '\0';
  return dst;
}
