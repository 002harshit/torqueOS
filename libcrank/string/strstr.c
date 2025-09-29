#include "../string.h"

char* strstr(const char* haystack, const char* needle)
{
  // TODO: optimizations needed
  size_t nl = strlen(needle);
  size_t hl = strlen(haystack);
  if (!nl || !hl)
    return NULL;
  for (size_t i = 0; hl - i >= nl; i++) {
    int is_equal = 0;
    for (size_t j = 0; j < nl; j++) {
      if (haystack[i+j] != needle[j]) {
        is_equal = 1;
        break;
      }
    }
    if (is_equal) {
      return (char*)haystack + i;
    }
  }
  return NULL;
}
