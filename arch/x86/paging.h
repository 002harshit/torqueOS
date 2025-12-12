#ifndef ARCH_X86_PAGING_H
#define ARCH_X86_PAGING_H

#define FRAME_SIZE 4096
#define PTE_COUNT 1024
#define PDE_COUNT 1024

struct page_entry_t {
  unsigned int present: 1;
  unsigned int rw:      1;
  unsigned int usr:     1;
  unsigned int zero:    9;
  unsigned int addr:   20;
} __attribute__((packed));

struct page_table_t {
  struct page_entry_t entries[PTE_COUNT];
} __attribute__((aligned(FRAME_SIZE)));

extern void load_page_dir(void* dir);
extern void invlpg_flush_page(void* virtual_address);

void paging_init();
void paging_enable();
void paging_map_page(unsigned int physical_address, unsigned int is_present, unsigned int is_writable, int should_flush);
void paging_map_region(unsigned int paddr_start, unsigned int paddr_end, unsigned int is_present, unsigned int is_writable, int should_flush);

#endif // ARCH_X86_PAGING_H
