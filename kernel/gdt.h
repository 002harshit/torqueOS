#ifndef GDT_H
#define GDT_H

struct Gdt {
  unsigned short size;
  unsigned int address;
} __attribute__((packed));

struct GdtDescriptor {
  unsigned short limit_low;
  unsigned short base_low;
  unsigned char base_mid;
  unsigned char access_flag;
  unsigned char limit_and_flags; // 4 bits for limit and other 4 for flags
  unsigned char base_high;
} __attribute__((packed));

void gdt_init();

void gdt_load(struct Gdt gdt);
void gdt_load_registers();

#endif // GDT_H
