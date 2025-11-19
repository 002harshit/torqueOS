#ifndef DRIVER_PS2_H
#define DRIVER_PS2_H

#include <arch/x86/io.h>

// Reference: make sure to read those
// https://users.utcluj.ro/~baruch/sie/labor/PS2/PS-2_Mouse_Interface.htm
// https://www.eecg.utoronto.ca/~jayar/ece241_08F/AudioVideoCores/ps2/ps2.html
// https://wiki.osdev.org/I8042_PS/2_Controller#Second_PS/2_Port

#define PS2_STATUS_REGISTER 0x64
#define PS2_COMMAND_PORT 0x64
#define PS2_DATA_PORT 0x60

#define PS2_CONFIG_READ 0x20
#define PS2_CONFIG_WRITE 0x60

#define PS2_ENABLE_KEYBOARD 0xAE
#define PS2_ENABLE_MOUSE 0xA8

#define PS2_MOUSE_PASSTHROUGH 0xD4

// these commands must be passthrough if you want to send it to mouse controller
#define PS2_RESETS_NO_TEST 0xF6
#define PS2_ENABLE_REPORTING 0xF4

// TODO: Create a general ps/2 controller driver
// because logic to initialize mouse and keyboard is similar
// also, we could also control a20 line using ps/2

static inline void ps2_outb(unsigned char port, unsigned char cmd)
{
  while (inb(PS2_STATUS_REGISTER) & 0b10) {} // waiting until bit 1 is not set
  outb(port, cmd);
}

static inline unsigned char ps2_inb(unsigned char port)
{
  while (!(inb(PS2_STATUS_REGISTER) & 0b01)) {} // waiting until bit 0 is set
  return inb(port);
}

#endif // DRIVER_PS2_H
