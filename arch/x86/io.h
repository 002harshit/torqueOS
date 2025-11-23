#ifndef IO_H
#define IO_H

void outb(unsigned short port, unsigned char data);
unsigned char inb(unsigned short port);
void io_wait(void);

#endif // IO_H
