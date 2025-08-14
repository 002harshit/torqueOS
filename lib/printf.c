#include "../driver/serial.h"
#include "../driver/framebuffer.h"
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
#define va_list __builtin_va_list

static void lib_putchar(char c)
{
  fb_write(c);
  // serial_write(c);
}

static void lib_puts(char* buf)
{
  char* p = buf;
  while (*p != '\0') {
    lib_putchar(*p);
    p++;
  }
}

static void print_int(unsigned int v, int base)
{
  char buffer[36];
  int len = 0;
  do {
    buffer[len++] = '0' + (v % base);
    v = v / base;
  } while(v > 0);
  for (int i = len-1; i > -1; i--) {
    lib_putchar(buffer[i]);
  }
}

static void print_hex(unsigned int v, const char* symbols)
{
  char buffer[12];
  int len = 0;
  do {
    buffer[len++] = symbols[v % 16];
    v = v / 16;
  } while (v > 0);
  for (int i = len-1; i > -1; i--) {
    lib_putchar(buffer[i]);
  }
}

// orintf
// support 'c': char, 's': char*, 'd': int, 'u': unsigend int, 'f': float, 'h': hex uint, 'b': hex uint

#define PRINTF_FLOAT_PRECISION 100000000

void lib_printf(const char* fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  for (const char* p = fmt; p[0] != '\0'; p++) {
    if (*p != '%') {
      lib_putchar(p[0]);
      continue;
    }
    p++;
    if (p[0] == '\0')
      break;
    switch(p[0]) {
      case 'c': {
        int arg = va_arg(va, int);
        if (arg != '\0')
        lib_putchar(arg);
      } break;
      case 's': {

      char* arg = va_arg(va, char*);
        lib_puts(arg);
      } break;

      case 'd': {
        int arg = va_arg(va, int);
        if (arg < 0) {
          lib_putchar('-');
          arg = -arg;
        }
        print_int(arg, 10);
      } break;
      
      case 'X':
      case 'x': {
        unsigned int arg = va_arg(va, unsigned int);
        const char* hex_symbols =  p[0] == 'x' ? "0123456789abcdef" : "0123456789ABCDEF";
        lib_puts("0x");
        print_hex(arg, hex_symbols);
      } break;

      case 'b': {
        unsigned int arg = va_arg(va, unsigned int);
        lib_puts("0b");
        print_int(arg, 2);
      } break;

      // WARN: might not be the ideal implementation, im just raw dogging it 
      case 'f': {
        double arg = va_arg(va, double);
        if (arg < 0) {
          lib_putchar('-');
          arg = -arg;
        }
        int iarg = arg;
        print_int(iarg, 10);
        arg = arg - (double)iarg;
        lib_putchar('.');
        print_int(arg * PRINTF_FLOAT_PRECISION, 10);
      } break;

      default: {
        lib_putchar(p[0]);
      }
    }
  }
  va_end(va);
}

