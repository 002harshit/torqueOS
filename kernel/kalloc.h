#ifndef KALLOC_H
#define KALLOC_H

#error "kalloc.h is deprecated, use allocator.h instead"

unsigned int sbrk(unsigned int increment);
void brk(unsigned int addr);
void* kalloc(unsigned int size, int should_align);

#endif // KALLOC_H
