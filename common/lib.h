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

#endif // COMMON_LIB_H
