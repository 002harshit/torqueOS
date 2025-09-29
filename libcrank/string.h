#ifndef CRANK_STRING_H
#define CRANK_STRING_H

#include "./types.h"

size_t strlen(const char* s);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
char* strcpy(char* restrict dst, const char* restrict src);
char* strncpy(char* restrict dst, const char* restrict src, size_t n);
char* strchr(const char* s, int c);
char* strrchr(const char* s, int c);
char* strstr(const char* haystack, const char* needle);

#endif // CRANK_STRING_H

