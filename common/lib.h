#ifndef COMMON_LIB_H
#define COMMON_LIB_H

#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
#define va_list __builtin_va_list

// INFO: lib_putchar must be defined seperatly in kernel and user space
void lib_putchar(char c);

void lib_puts(char* buf);
void lib_printf(const char* fmt, ...);



void *memcpy(void* restrict dest, const void* restrict src, unsigned int n);
void *memmove(void* dest, const void* src, unsigned int n);
void *memset(void* s, int c, unsigned int n);
int memcmp(const void* s1, const void* s2, unsigned int n);





#endif // COMMON_LIB_H
