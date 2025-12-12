#include <libcrank/std.h>

#include "./paging.h"

static struct page_entry_t _kernel_directory[PDE_COUNT] __attribute__((aligned(FRAME_SIZE)));
static struct page_table_t _kernel_tables[PDE_COUNT] __attribute__((aligned(FRAME_SIZE)));

void paging_init()
{
  for (unsigned int i = 0; i < PDE_COUNT; i++) {
    _kernel_directory[i].present = 0;
    for (unsigned int j = 0; j < PTE_COUNT; j++) {
      _kernel_tables[i].entries[j].present = 0;
    }
  }
}

void paging_enable()
{
  load_page_dir(&_kernel_directory);
  printf("[INFO] Enabled paging\n");
}

void paging_map_page(unsigned int physical_address, unsigned int is_present, unsigned int is_writable, int should_flush)
{
  // INFO: strictly assumes identity paging
  unsigned int frame_index = physical_address / FRAME_SIZE;
  unsigned int pde_index = frame_index / PTE_COUNT;
  unsigned int pte_index = frame_index % PTE_COUNT;

  struct page_entry_t* table_entry = &_kernel_directory[pde_index];
  if (!table_entry->present) {
    table_entry->present = 1;
    table_entry->rw = 1;
    table_entry->usr = 0;
    table_entry->zero = 0;
    table_entry->addr = ((unsigned int) &_kernel_tables[pde_index]) >> 12;
  }
  struct page_entry_t* entry = &_kernel_tables[pde_index].entries[pte_index];

  // INFO: structure of page entry
  // | 0       | 1  | 2   | 3 - 11 | 12 - 31 |
  // | present | rw | usr | zero   | addr    |
  entry->present = is_present ? 1 : 0;
  entry->rw = is_writable ? 1 : 0;
  entry->usr = 0;
  entry->zero = 0;
  entry->addr = physical_address >> 12;

  if (should_flush) {
    unsigned int vaddr = frame_index * FRAME_SIZE;
    invlpg_flush_page((void*) vaddr);
  }
}

void paging_map_region(unsigned int paddr_start, unsigned int paddr_end, unsigned int is_present, unsigned int is_writable, int should_flush)
{
  paddr_start = paddr_start & ~(FRAME_SIZE - 1);
  paddr_end = (paddr_end + FRAME_SIZE - 1) & ~(FRAME_SIZE - 1);

  for (unsigned int addr = paddr_start; addr < paddr_end; addr += FRAME_SIZE) {
    paging_map_page(addr, is_present, is_writable, should_flush);
  }
}

void on_page_fault() {
  printf("PAGE FAULTED\n");
  while (1) {}
}
