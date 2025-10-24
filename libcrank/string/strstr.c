#include "../string.h"

char* strstr(const char* haystack, const char* needle)
{
  // OPTIMIZE: using the “first character match” trick
  size_t nl = strlen(needle);
  if (!nl)
    return (char*) haystack;

  size_t hl = strlen(haystack);
  if (!hl)
    return NULL;
 for (size_t i = 0; hl - i >= nl; i++) {
    int is_equal = 1;
    for (size_t j = 0; j < nl; j++) {
      if (haystack[i+j] != needle[j]) {
        is_equal = 0;
        break;
      }
    }
    if (is_equal) {
      return (char*)haystack + i;
    }
  }
  return NULL;
}
