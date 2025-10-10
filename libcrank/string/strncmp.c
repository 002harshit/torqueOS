#include "../string.h"

int strncmp(const char *s1, const char *s2, size_t n)
{
  for (; n; n--) {
    char a = *s1++;
    char b = *s2++;
    if (a != b)
      return a - b;
    if (a == '\0')
      return 0;
  }
  return 0;
}
