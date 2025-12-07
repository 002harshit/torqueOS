#ifndef ARCH_X86_GDT_H
#define ARCH_X86_GDT_H

struct gdt_t {
  unsigned short size;
  unsigned int address;
} __attribute__((packed));

struct gdt_descriptor_t {
  unsigned short limit_low;
  unsigned short base_low;
  unsigned char base_mid;
  unsigned char access_flag;
  unsigned char limit_and_flags; // 4 bits for limit and other 4 for flags
  unsigned char base_high;
} __attribute__((packed));

void gdt_init();

void gdt_load(struct gdt_t gdt);
void gdt_load_registers();

#endif // ARCH_X86_GDT_H
