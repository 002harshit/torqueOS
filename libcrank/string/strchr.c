#include "../string.h"

char* strchr(const char* s, int c)
{
  if (c == '\0')
    return (char*)s + strlen(s);
  for (; *s; s++) {
    if (*s == c)
      return (char*)s;
  }
  return NULL;
}
