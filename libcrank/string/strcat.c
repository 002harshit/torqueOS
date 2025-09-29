#include "../string.h"

char* strcat(char *restrict dst, const char *restrict src)
{
  char* d = dst;
  for (; *d; d++) {}
  for (; *src; ) {
    *d++ = *src++;
  }
  *d = '\0';
  return dst; 
}
