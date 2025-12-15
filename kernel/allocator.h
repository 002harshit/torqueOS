#ifndef KERNEL_ALLOCATOR_H
#define KERNEL_ALLOCATOR_H

#include "./multiboot2.h"

#define MAX_32BIT_ADDRESS (0xFFFFFFFF)

typedef struct {
  unsigned long long start;
  unsigned long long end;
} mem_region_t;

enum {
  MEM_BLOCK_FREE = 0b0,
  MEM_BLOCK_USED = 0b1,
  MEM_BLOCK_RESTRICTED = 0b11
};
typedef unsigned int mem_block_state_t;

typedef struct {
  unsigned int addr, len;
  mem_block_state_t state;
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
