#include "../string.h"

size_t strlen(const char *s)
{
  size_t l = 0;
  for (; s[l]; l++) {}
  return l;
}

