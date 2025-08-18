#include "serial.h"
#include "kernel/io.h"  

#define SERIAL_COM1_BASE 0x3F8

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

/* The I/O port commands */
#define SERIAL_LINE_ENABLE_DLAB 0x80

static void serial_configure_baud_rate(unsigned short com, unsigned short divisor)
{
  outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
  outb(SERIAL_DATA_PORT(com), (divisor >> 8) & 0x00FF);
  outb(SERIAL_DATA_PORT(com), divisor & 0x00FF);
}

static void serial_configure_line(unsigned short com)
{
  outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}

static void serial_configure_fifo(unsigned short com)
{
  outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);
}

static void serial_configure_modem(unsigned short com)
{
  outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}

void serial_init()
{
  serial_configure_baud_rate(SERIAL_COM1_BASE, 2);
  serial_configure_line(SERIAL_COM1_BASE);
  serial_configure_fifo(SERIAL_COM1_BASE);
  serial_configure_modem(SERIAL_COM1_BASE);
}

static int serial_is_transmit_fifo_empty(unsigned int com)
{
  return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

void serial_write(char c)
{
  while (!serial_is_transmit_fifo_empty(SERIAL_COM1_BASE)) {}
  outb(SERIAL_DATA_PORT(SERIAL_COM1_BASE), c);
}
