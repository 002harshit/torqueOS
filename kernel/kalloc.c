extern unsigned int _kernel_end;
static unsigned int alloc_top = (unsigned int) &_kernel_end + 0x400 * 0x400;

#define ALIGN_UP(addr, align) ((addr) + ((align) - 1)) & ~((align) - 1)

unsigned int sbrk(unsigned int size)
{
  if (!size) {
    return alloc_top;
  }
  unsigned int tmp = alloc_top;
  alloc_top += size;
  return tmp;
}

void brk(unsigned int addr)
{
  alloc_top = addr;
}

#include <arch/x86/paging.h>

void* kalloc(unsigned int size, int align)
{
  if (align) {
    brk(ALIGN_UP(alloc_top, align));
  }
  paging_map_region(alloc_top, alloc_top + size, 1, 1, 1);
  return (void*) sbrk(size);
}
