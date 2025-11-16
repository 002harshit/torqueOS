#ifndef IDT_H
#define IDT_H
struct CpuState {
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
	unsigned int ebp;
	unsigned int esi;
	unsigned int edi;
} __attribute__((packed));

struct StackState {
	unsigned int error_code;
	unsigned int eip;
	unsigned int cs;
	unsigned int eflags;
} __attribute__((packed));

struct Idt {
	unsigned short size;
	unsigned int address;
} __attribute__((packed));

struct IdtDescriptor {
	unsigned short offset_low;
	unsigned short segment_selector;
	unsigned char zero;
	unsigned char flags;
	unsigned short offset_high;
} __attribute__((packed));

void idt_init();

void load_idt(unsigned int idt_address);
#endif // IDT_H
