#include "driver/serial.h"
#include "driver/framebuffer.h"
int threesome(int a, int b, int c) {
	return a + b + c;
}

void lib_printf(const char* fmt, ...);

void demo()
{
///	char* message = "HelloWorld";
	// serial_init();
  fb_clear_screen();
	lib_printf("Hello %c\n%s\n%f %d %X %b %b %f\n", ',', "World", 78, -12, -44, 12, 0, -56.24435435);
}
