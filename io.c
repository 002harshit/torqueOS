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
