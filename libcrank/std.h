#ifndef CRANK_STD_H
#define CRANK_STD_H

#include "./types.h"

void *memcpy(void* restrict dest, const void* restrict src, size_t n);
void *memset(void* s, int c, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);

#endif // CRANK_STD_H
