#include "timer.h"
#include "kernel/io.h"  

static void _default_handler() {}
static TimerCallback timer_handler = _default_handler;
static unsigned int delta_ms = 0;
static unsigned long elapsed = 0;

void timer_start(int frequency)
{
  delta_ms = 1000 / frequency;
  elapsed = 0;

  unsigned int divisor = 1193180 / frequency;
  unsigned char l = divisor & 0xFF;
  unsigned char r = (divisor >> 8) & 0xFF;
  outb(0x43, 0x36);
  outb(0x40, l);
  outb(0x40, r);
}

void timer_set_callback(TimerCallback handler)
{
  timer_handler = handler;
}

void on_timer_interrupt()
{
  elapsed += delta_ms;
  timer_handler();
}

void timer_stop()
{
  delta_ms = 0;
  outb(0x43, 0x30);
  outb(0x40, 0);
  outb(0x40, 0);
}

unsigned long timer_get_elapsed()
{
  return elapsed;
}
