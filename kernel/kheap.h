#ifndef KHEAP_H
#define KHEAP_H

unsigned int kmalloc_a(unsigned int size);
unsigned int kmalloc_p(unsigned int size, unsigned int *phys);
unsigned int kmalloc_ap(unsigned int size, unsigned int *phys);
unsigned int kmalloc(unsigned int size);

extern unsigned int placement_address;

#endif // KHEAP_H
