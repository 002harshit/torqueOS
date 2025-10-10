#include "./kheap.h"
#include "./paging.h"
#include "libcrank/types.h"

// INFO: we are only interested in address of _kernel_end and not its value stored innit
extern unsigned int _kernel_end;
unsigned int placement_address = (unsigned int)&_kernel_end;

static unsigned int _kmalloc(unsigned int size, int align, unsigned int* phys)
{
  if (align) {
    placement_address = (placement_address + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
  }
  if (phys) {
    *phys = placement_address;
  }
  unsigned int tmp = placement_address;
  placement_address += size;
  return tmp;
}

unsigned int kmalloc_a(unsigned int size)
{
  return _kmalloc(size, 1, NULL);
}

unsigned int kmalloc_p(unsigned int size, unsigned int *phys)
{
  return _kmalloc(size, 0, phys);
}

unsigned int kmalloc_ap(unsigned int size, unsigned int *phys)
{
  return _kmalloc(size, 1, phys);
}

unsigned int kmalloc(unsigned int size)
{
  return _kmalloc(size, 1, NULL);
}
