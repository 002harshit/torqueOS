#ifndef KALLOC_H
#define KALLOC_H

unsigned int sbrk(unsigned int increment);
void brk(unsigned int addr);
void* kalloc(unsigned int size, int should_align);

#endif // KALLOC_H
