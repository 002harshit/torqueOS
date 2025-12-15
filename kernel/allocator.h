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

  // INFO: i can easily pack these booleans in a single byte to space
  // but it might not work when defining linear array of them due to alignment issues
  // another solution would be to use 'struct of array' instead of 'array of struct' but it will introduce complexity
  int is_used, is_restricted;
} mem_block_t;

typedef struct {
  mem_block_t* blocks;
  unsigned int blocks_cap;
  unsigned int blocks_len;
} mem_allocator_t;


void allocator_init(struct multiboot_tag_mmap* mmap_tag, mem_region_t* used_regions, unsigned int used_regions_len);
void* allocator_malloc(unsigned int size);
int allocator_free(void* ptr);
void* allocator_realloc(void* old_ptr, unsigned int new_size);

#endif // KERNEL_ALLOCATOR_H
