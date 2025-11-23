#include "./io.h"

// INFO: https://stackoverflow.com/questions/6793899/what-does-the-0x80-port-address-connect-to
// port 0x80 is normally used by bios for POST tests
// but in kernel space we can use it to arbitary wait for an io operations
void io_wait()
{
	__asm__ __volatile__ (
		"out %[port], %[data]"
		:
		: [port] "d" (0x80), [data] "a" (0)
    :
	);
}

unsigned char inb(unsigned short port) {
  unsigned char data;
  __asm__ __volatile__ (
    "in %[data], %[port]"
    : [data] "=a" (data)
    : [port] "d" (port)
    :
  );
  return data;
}

void outb(unsigned short port, unsigned char data)
{
	__asm__ __volatile__ (
		"out %[port], %[data]"
		:
		: [port] "d" (port), [data] "a" (data)
    :
	);
}
