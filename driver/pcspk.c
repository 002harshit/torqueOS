#include <arch/x86/io.h>

#include "./pcspk.h"

  // INFO: PCSPK just sets the frequency at which the cpu will produce sound
  // PCSPK does not support fancy features like multiple channels or fourier transform 
  // that you take for granted when working with your modern sound card
  
  // INFO: one fun thing i could do is convert MIDI files into one channeled raw frequency, time data
  // and changes the frequency of PCSPK based on it using PIT timer to play a song.
  // some people managed to play 'Bad Apple' using similar idea

void pcspk_start(int frequency)
{
  unsigned int divisor = 1193180 / frequency;
  unsigned char l = divisor & 0xFF;
  unsigned char r = (divisor >> 8) & 0xFF;

  outb(0x43, 0xb6);
  outb(0x42, l);
  outb(0x42, r);

  unsigned int nmi_status = inb(0x61);
  nmi_status = nmi_status | 0b11;
  outb(0x61, nmi_status);
}

void pcspk_stop()
{
  unsigned int nmi_status = inb(0x61);
  nmi_status = nmi_status & 0x01;
  outb(0x61, nmi_status);
}
