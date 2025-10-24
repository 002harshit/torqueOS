#ifndef CRANK_STD_H
#define CRANK_STD_H

#include "./types.h"

#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
#define va_list __builtin_va_list

#define PRINTF_FLOAT_PRECISION 100000000

// INFO: lib_putchar must be defined seperatly in kernel and user space
void putchar(char c);
void puts(char* buf);
void printf(const char* fmt, ...);

void *memcpy(void* restrict dest, const void* restrict src, size_t n);
void *memset(void* s, int c, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);

#endif // CRANK_STD_H
