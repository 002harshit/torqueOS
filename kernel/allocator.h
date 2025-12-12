#ifndef KERNEL_ALLOCATOR_H
#define KERNEL_ALLOCATOR_H

#include "./multiboot2.h"

#define MAX_32BIT_ADDRESS (0xFFFFFFFF)

typedef struct {
  unsigned long long start;
  unsigned long long end;
} mem_region_t;

typedef struct {
  unsigned long long addr, len;
  unsigned long long is_used;
} block_t;

void allocator_init(struct multiboot_tag_mmap* mmap_tag, mem_region_t* used_regions, unsigned int used_regions_len);

#endif // KERNEL_ALLOCATOR_H
