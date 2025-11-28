#include "./io.h"
#include "./idt.h"

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

#define PIC_INTERRUPT(code) (PIC_1_OFFSET + code)
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
		outb(PIC_1_COMMAND_PORT, PIC_ACKNOWLEDGE);
	}
}

void pic_remap(int offset1, int offset2){
	// starts the initialization sequence (in cascade mode)
	outb(PIC_1_COMMAND, PIC_ICW1_INIT + PIC_ICW1_ICW4);
  io_wait();

	outb(PIC_2_COMMAND, PIC_ICW1_INIT + PIC_ICW1_ICW4);
  io_wait();

	outb(PIC_1_DATA, offset1);
  io_wait();

	outb(PIC_2_DATA, offset2);
  io_wait();

	outb(PIC_1_DATA, 4);
  io_wait();

	outb(PIC_2_DATA, 2);
  io_wait();

	outb(PIC_1_DATA, PIC_ICW4_8086);
	outb(PIC_2_DATA, PIC_ICW4_8086);

	// Setup Interrupt Mask Register (IMR)
  // 1=masked, 0=unmasked
  #define MASK_TIMER    0b11111110
  #define MASK_KEYBOARD 0b11111101
  #define MASK_IRQ2     0b11111011 /* IRQ2 must be enabled for ps/2 mouse to work */
  #define MASK_MOUSE    0b11101111
	outb(PIC_1_DATA, MASK_IRQ2 & MASK_KEYBOARD & MASK_TIMER);
	outb(PIC_2_DATA, MASK_MOUSE);

	__asm__ __volatile__("sti");
}

extern void on_timer_interrupt();
extern void on_keyboard_interrupt();
extern void on_mouse_interrupt();
extern void on_page_fault();

void interrupt_handler(struct interrupt_cpu_state_t* regs, unsigned int interrupt_code, struct interrupt_stack_state_t* stack)
{
  switch (interrupt_code) {
    case PIC_INTERRUPT(0): {
      on_timer_interrupt();
    } break;
    case PIC_INTERRUPT(1): {
      on_keyboard_interrupt();
    } break;
    case 44: {
      on_mouse_interrupt();
    } break;
    case 14: {
      on_page_fault();
    } break;
  }
  pic_acknowledge(interrupt_code);
}

static struct idt_descriptor_t descriptors[IDT_DESCRIPTORS_COUNT] = {0};

static void set_idt(int index, unsigned int offset, unsigned char flags, unsigned short segment_selector)
{
	descriptors[index].offset_low = offset & 0xffff;
	descriptors[index].segment_selector = segment_selector;
	descriptors[index].zero = 0;
	descriptors[index].flags = flags;
	descriptors[index].offset_high = (offset >> 16) & 0xffff;
}

extern void interrupt_handler_33();
extern void interrupt_handler_32();
extern void interrupt_handler_44();
extern void interrupt_handler_14();

static struct idt_t idt;

void idt_init()
{
  set_idt(32, (unsigned int) interrupt_handler_32, IDT_FLAGS, 0x08);
  set_idt(33, (unsigned int) interrupt_handler_33, IDT_FLAGS, 0x08);
  set_idt(44, (unsigned int) interrupt_handler_44, IDT_FLAGS, 0x08);
  set_idt(14, (unsigned int) interrupt_handler_14, IDT_FLAGS, 0x08);

  idt.address = (unsigned int ) &descriptors;
  idt.size = sizeof(struct idt_descriptor_t) * IDT_DESCRIPTORS_COUNT - 1;
  load_idt((unsigned int)&idt);
  pic_remap(PIC_1_OFFSET, PIC_2_OFFSET);
}
