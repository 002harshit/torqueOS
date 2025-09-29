#include "../string.h"

char* strrchr(const char* s, int c)
{
  size_t l = strlen(s);
  if (c == '\0')
    return (char*)s + l;
  for (; l; l--) {
    if (s[l] == c)
      return (char*)s;
  }
  return NULL;
}


