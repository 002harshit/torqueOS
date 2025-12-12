#include "../std.h"


extern void putchar(char c);

void puts(char* buf)
{
  char* p = buf;
  while (*p != '\0') {
    putchar(*p);
    p++;
  }
}

static void print_int(unsigned int v, int base)
{
  char buffer[40];
  int len = 0;
  do {
    buffer[len++] = '0' + (v % base);
    v = v / base;
  } while(v > 0);
  for (int i = len-1; i > -1; i--) {
    putchar(buffer[i]);
  }
}

static void print_int64(unsigned long long v, int base)
{
  char buffer[72];
  int len = 0;
  do {
    buffer[len++] = '0' + (v % base);
    v = v / base;
  } while(v > 0);
  for (int i = len-1; i > -1; i--) {
    putchar(buffer[i]);
  }
}

static void print_hex(unsigned int v, const char* symbols)
{
  char buffer[16];
  int len = 0;
  do {
    buffer[len++] = symbols[v % 16];
    v = v / 16;
  } while (v > 0);
  for (int i = len-1; i > -1; i--) {
    putchar(buffer[i]);
  }
}

static void print_hex64(unsigned long long v, const char* symbols)
{
  char buffer[24];
  int len = 0;
  do {
    buffer[len++] = symbols[v % 16];
    v = v / 16;
  } while (v > 0);
  for (int i = len-1; i > -1; i--) {
    putchar(buffer[i]);
  }
}

void printf(const char* fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  for (const char* p = fmt; p[0] != '\0'; p++) {
    if (*p != '%') {
      putchar(p[0]);
      continue;
    }
    p++;
    if (p[0] == '\0')
      break;
    switch(p[0]) {
      case 'c': {
        int arg = va_arg(va, int);
        if (arg != '\0')
        putchar(arg);
      } break;
      case 's': {

      char* arg = va_arg(va, char*);
        puts(arg);
      } break;

      case 'l': {
        p++;
        if (p[0] == 'd') {
          long long arg = va_arg(va, long long);
          if (arg < 0) {
            putchar('-');
            arg = -arg;
          }
          print_int64(arg, 10);
        } else if (p[0] == 'u') {
          unsigned long long arg = va_arg(va, unsigned long long);
          print_int64(arg, 10);
        } else if (p[0] == 'x' || p[0] == 'X') {
          unsigned long long arg = va_arg(va, unsigned long long);
          const char* hex_symbols =  p[1] == 'x' ? "0123456789abcdef" : "0123456789ABCDEF";
          puts("0x");
          print_hex64(arg, hex_symbols);
        } else if (p[0] == 'b') {
          unsigned long long arg = va_arg(va, unsigned long long);
          puts("0b");
          print_int64(arg, 2);
        } else {
          puts("%l");
          putchar(p[0]);
        }
      } break;

      case 'd': {
        int arg = va_arg(va, int);
        if (arg < 0) {
          putchar('-');
          arg = -arg;
        }
        print_int(arg, 10);
      } break;

      case 'u': {
        unsigned int arg = va_arg(va, unsigned int);
        print_int(arg, 10);
      } break;
 
      case 'X':
      case 'x': {
        unsigned int arg = va_arg(va, unsigned int);
        const char* hex_symbols =  p[0] == 'x' ? "0123456789abcdef" : "0123456789ABCDEF";
        puts("0x");
        print_hex(arg, hex_symbols);
      } break;

      case 'b': {
        unsigned int arg = va_arg(va, unsigned int);
        puts("0b");
        print_int(arg, 2);
      } break;

      // WARN: might not be the ideal implementation, im just raw dogging it 
      case 'f': {
        double arg = va_arg(va, double);
        if (arg < 0) {
          putchar('-');
          arg = -arg;
        }
        int iarg = arg;
        print_int(iarg, 10);
        arg = arg - (double)iarg;
        putchar('.');
        print_int(arg * PRINTF_FLOAT_PRECISION, 10);
      } break;

      default: {
        putchar('%');
        putchar(p[0]);
      }
    }
  }
  va_end(va);
}

