#ifndef KHEAP_H
#define KHEAP_H

typedef struct block_info_t {
  unsigned int size;
  int is_free;
  struct block_info_t* next;
} block_info_t;

block_info_t* heap_create();
unsigned int kmalloc_a(unsigned int size);
unsigned int kmalloc_p(unsigned int size, unsigned int *phys);
unsigned int kmalloc_ap(unsigned int size, unsigned int *phys);
unsigned int kmalloc(unsigned int size);

extern unsigned int placement_address;
extern block_info_t* kheap;

#endif // KHEAP_H
