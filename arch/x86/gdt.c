#include <arch/x86/gdt.h>

enum {
  SEGMENT_INDEX_NULL = 0,
  SEGMENT_INDEX_CODE = 1,
  SEGMENT_INDEX_DATA = 2,
  SEGMENT_COUNT,
};

#define SEGMENT_ACCESS_FLAG_CODE 0x9a
#define SEGMENT_ACCESS_FLAG_DATA 0x92

#define SEGMENT_BASE 0x0
#define SEGMENT_FLAGS 0xc

#define SEGMENT_LIMIT 0xfffff

static struct gdt_descriptor_t gdt_descriptors[SEGMENT_COUNT];
static struct gdt_t gdt;

static void set_descriptor(int index, unsigned int base, unsigned int limit, unsigned char access_flag, unsigned char flags)
{
  gdt_descriptors[index].base_low  = base & 0xffff;
  gdt_descriptors[index].base_mid  = (base >> 16) & 0xff;
  gdt_descriptors[index].base_high = (base >> 24) & 0xff;

  gdt_descriptors[index].limit_low = limit & 0xffff;
  gdt_descriptors[index].limit_and_flags = ((limit >> 16) & 0xf) | ((flags << 4) & 0xf0);

  gdt_descriptors[index].access_flag = access_flag;
}

void gdt_init()
{
  set_descriptor(SEGMENT_INDEX_NULL, 0x0, 0x0, 0x0, 0x0);
  set_descriptor(SEGMENT_INDEX_CODE, SEGMENT_BASE, SEGMENT_LIMIT, SEGMENT_ACCESS_FLAG_CODE, SEGMENT_FLAGS);
  set_descriptor(SEGMENT_INDEX_DATA, SEGMENT_BASE, SEGMENT_LIMIT, SEGMENT_ACCESS_FLAG_DATA, SEGMENT_FLAGS);

  gdt.address = (unsigned int) gdt_descriptors; 

  // INFO: lgdt instruction expects 'table limit'
  // the table limit specifies the number of bytes in the table which is (table_size - 1)
  gdt.size = sizeof(struct gdt_descriptor_t) * SEGMENT_COUNT - 1;

  gdt_load(gdt);
  gdt_load_registers();
}




