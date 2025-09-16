#include "io.h"
#include "idt.h"
#include "common/lib.h"

#define PIC_1 0x20
#define PIC_2 0xA0
#define PIC_1_COMMAND	PIC_1
#define PIC_1_DATA (PIC_1+1)
#define PIC_2_COMMAND	PIC_2
#define PIC_2_DATA (PIC_2+1)

#define PIC_1_OFFSET 0x20
#define PIC_2_OFFSET 0x28
#define PIC_2_END PIC_2_OFFSET + 7

#define PIC_1_COMMAND_PORT 0x20
#define PIC_2_COMMAND_PORT 0xA0
#define PIC_ACKNOWLEDGE 0x20

#define PIC_ICW1_ICW4 0x01
#define PIC_ICW1_INIT 0x10

#define PIC_ICW4_8086 0x01

#define PIC_1_INTERRUPT(code) (PIC_1_OFFSET + code)
#define CODE_SEGMENT_OFFSET 0x08

#define IDT_DESCRIPTORS_COUNT 256

/* P = 1, DPL = 0, D = 0 */
#define IDT_FLAGS 0x8e

void pic_acknowledge(unsigned int interrupt){
	if (interrupt < PIC_1_OFFSET || interrupt > PIC_2_END) {
		return;
	}

	if (interrupt < PIC_2_OFFSET) {
		outb(PIC_1_COMMAND_PORT, PIC_ACKNOWLEDGE);
	} else {
		outb(PIC_2_COMMAND_PORT, PIC_ACKNOWLEDGE);
	}
}

void pic_remap(int offset1, int offset2){
	// starts the initialization sequence (in cascade mode)
	outb(PIC_1_COMMAND, PIC_ICW1_INIT + PIC_ICW1_ICW4);
	outb(PIC_2_COMMAND, PIC_ICW1_INIT + PIC_ICW1_ICW4);
	outb(PIC_1_DATA, offset1);
	outb(PIC_2_DATA, offset2);
	outb(PIC_1_DATA, 4);
	outb(PIC_2_DATA, 2);

	outb(PIC_1_DATA, PIC_ICW4_8086);
	outb(PIC_2_DATA, PIC_ICW4_8086);

	// Setup Interrupt Mask Register (IMR)
	outb(PIC_1_DATA, 0b11111100);
	outb(PIC_2_DATA, 0xFF);

	__asm__("sti");
}

extern void on_timer_interrupt();
extern void on_keyboard_interrupt();
extern void on_page_fault();

void interrupt_handler(__attribute__((unused)) struct CpuState cpu, unsigned int interrupt, __attribute__((unused)) struct StackState stack)
{
  switch (interrupt) {
    case PIC_1_INTERRUPT(0): {
      on_timer_interrupt();
    } break;
    case PIC_1_INTERRUPT(1): {
      on_keyboard_interrupt();
    } break;
    case 14: {
      on_page_fault();
    } break;
  }
  pic_acknowledge(interrupt);
}

static struct IdtDescriptor idt_descriptors[IDT_DESCRIPTORS_COUNT];

static void set_idt(int index, unsigned int offset, unsigned char flags, unsigned short segment_selector)
{
	idt_descriptors[index].offset_low = offset & 0xffff;
	idt_descriptors[index].segment_selector = segment_selector;
	idt_descriptors[index].zero = 0;
	idt_descriptors[index].flags = flags;
	idt_descriptors[index].offset_high = (offset >> 16) & 0xffff;
}

/* PIC_1_OFFSET + 1 = 33 */
extern void interrupt_handler_33();
extern void interrupt_handler_32();
extern void interrupt_handler_14();

static struct Idt idt;

void idt_init()
{
  set_idt(32, (unsigned int) interrupt_handler_32, IDT_FLAGS, 0x08);
  set_idt(33, (unsigned int) interrupt_handler_33, IDT_FLAGS, 0x08);
  set_idt(14, (unsigned int) interrupt_handler_14, IDT_FLAGS, 0x08);

  idt.address = (unsigned int ) &idt_descriptors;
  idt.size = sizeof(struct IdtDescriptor) * IDT_DESCRIPTORS_COUNT;
  load_idt((unsigned int)&idt);
  pic_remap(PIC_1_OFFSET, PIC_2_OFFSET);
}
