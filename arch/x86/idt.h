#ifndef IDT_H
#define IDT_H

struct interrupt_cpu_state_t {
	unsigned int edi;
	unsigned int esi;
	unsigned int ebp;
	unsigned int edx;
	unsigned int ecx;
	unsigned int ebx;
  unsigned int eax;
} __attribute__((packed));

struct interrupt_stack_state_t {
	unsigned int error_code;
	unsigned int eip;
	unsigned int cs;
	unsigned int eflags;
} __attribute__((packed));

struct idt_t {
	unsigned short size;
	unsigned int address;
} __attribute__((packed));

struct idt_descriptor_t {
	unsigned short offset_low;
	unsigned short segment_selector;
	unsigned char zero;
	unsigned char flags;
	unsigned short offset_high;
} __attribute__((packed));

void idt_init();

void load_idt(unsigned int idt_address);

#endif // IDT_H
