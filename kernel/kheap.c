#include "./kheap.h"
#include "./paging.h"
#include "libcrank/types.h"

// INFO: we are only interested in address of _kernel_end and not its value stored innit
extern unsigned int _kernel_end;
unsigned int placement_address = (unsigned int)&_kernel_end;

#define BLOCK_SIZE PAGE_SIZE
#define INITIAL_HEAP_SIZE BLOCK_SIZE
#define ALIGN_UP(addr, align) ((addr) + ((align) - 1)) & ~((align) - 1)

block_info_t* kheap = NULL;

block_info_t* heap_create()
{
  block_info_t* heap = (block_info_t*) sbrk(INITIAL_HEAP_SIZE);
  heap->size = INITIAL_HEAP_SIZE - sizeof(block_info_t);
  heap->is_free = 1;
  heap->next = NULL;
  return heap;
}

void heap_expand(size_t size, block_info_t* heap)
{
  size_t expand_size = ALIGN_UP(size + sizeof(block_info_t), BLOCK_SIZE);

  block_info_t* block = (block_info_t*) sbrk(expand_size);
  block->size = expand_size - sizeof(block_info_t);
  block->is_free = 1;
  block->next = NULL;

  block_info_t* curr = heap;
  while (curr->next) {
    curr = curr->next;
  }
  curr->next = block;
}

static block_info_t* find_best_block(size_t size, block_info_t* heap)
{
  block_info_t* best = NULL;
  for (block_info_t* curr = heap; curr; curr = curr->next) {
    if (!curr->is_free || curr->size < size) {
      continue;
    }
    if (!best || best->size > curr->size) {
      best = curr;
    }
  }
  return best;
}

void* alloc(size_t size, block_info_t* heap)
{
  // due to ABI reasons, align 8 bytes for x86 and 16 bytes for x86-64
  size = ALIGN_UP(size, 8);

  block_info_t* found = find_best_block(size, heap);
  if (found) {
    // TODO: create/shrink block according to size
    found->is_free = 0;
    return (void*) ((char*)found + sizeof(block_info_t));
  }

  heap_expand(size, heap);

  // try again after expanding
  found = find_best_block(size, heap);
  if (found) {
    // TODO: create/shrink block according to size
    found->is_free = 0;
    return (void*) ((char*)found + sizeof(block_info_t));
  }
  return NULL;
}

void free(void* ptr)
{
  if (!ptr) return;

  block_info_t* block = (block_info_t*)((char*)ptr - sizeof(block_info_t));
  block->is_free = 1;
  // printf("FREED: %p, %lu, %d\n", block, block->size, block->is_free);
}

static unsigned int _kmalloc(unsigned int size, int align, unsigned int* phys)
{
  if (kheap) {
    void* addr = alloc(size, kheap);
    if (phys) {
      page_entry_t *page = get_page((unsigned int)addr, 0, kernel_directory);
      *phys = (page->entry.frame * PAGE_SIZE) + ((unsigned int)addr & 0xFFF); // base + offset
    }
    return (unsigned int)addr;
  } else {
    if (align) {
      placement_address = ALIGN_UP(placement_address, PAGE_SIZE);
    }
    if (phys) {
      *phys = placement_address;
    }
    unsigned int tmp = placement_address;
    placement_address += size;
    return tmp;
  }
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
