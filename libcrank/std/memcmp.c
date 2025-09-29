#include "../std.h"

int memcmp(const void* s1, const void* s2, size_t n)
{
  const unsigned char* a = s1;
	const unsigned char* b = s2;
  for (; n; n--) {
    int res = (*a++) - (*b++);
    if (res != 0)
      return res;
  }
  return 0;
}
