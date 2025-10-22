#include "./paging.h"
#include "./kheap.h"
#include "common/lib.h"
#include "libcrank/std.h"

page_directory_t *current_directory;
page_directory_t *kernel_directory;

static unsigned int *frames;
static unsigned int frames_count;

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

static void set_frame(unsigned int frame_address)
{
  unsigned int frame = frame_address / PAGE_SIZE;
  unsigned int offset = OFFSET_FROM_BIT(frame);
  unsigned int index = INDEX_FROM_BIT(frame);
  frames[index] |= 0x1 << offset;
}

static void clear_frame(unsigned int frame_address)
{
  unsigned int frame = frame_address / PAGE_SIZE;
  unsigned int offset = OFFSET_FROM_BIT(frame);
  unsigned int index = INDEX_FROM_BIT(frame);
  frames[index] &= ~(0x1 << offset);
}

unsigned int test_frame(unsigned int frame_address)
{
  unsigned int frame = frame_address / PAGE_SIZE;
  unsigned int offset = OFFSET_FROM_BIT(frame);
  unsigned int index = INDEX_FROM_BIT(frame);
  return frames[index] & (0x1 << offset);
}

// Gets first free frame
static unsigned int get_first_frame()
{
  for (unsigned int i = 0; i < INDEX_FROM_BIT(frames_count); i++) {
    // if entire frame is full, continue
    if (frames[i] == (unsigned int) -1)
      continue;

    // check each bit in 32 bits value
    for (int j = 0; j < 32; j++) {
      unsigned int test = 0x1 << j;
      if (!(frames[i] & test)) {
        return (i * 4 * 8) + j;
      }
    }
  }
  return (unsigned int) -1;
}

page_entry_t *alloc_frame(page_entry_t *page, int is_kernel, int is_writeable)
{
  if (!page)
    return NULL;
  unsigned int index = get_first_frame();
  if (index == (unsigned int) -1) {
    // TODO: Replace this print with panic statement
    lib_printf("Unable to alloc free frame");
    while (1) {}
    return NULL;
  }
  set_frame(index * PAGE_SIZE);
  page->raw = 0;
  page->entry.present = 1;
  page->entry.rw = is_writeable ? 1 : 0;
  page->entry.user = is_kernel ? 0 : 1;
  page->entry.frame = index;
  return page;
}

void free_frame(page_entry_t *page)
{
  if (!(page->entry.frame))
    return;
  clear_frame(page->entry.frame);
  page->entry.frame = 0;
}

void switch_page_directory(page_directory_t *dir)
{
  current_directory = dir;
  load_page_directory(dir->tables_physical);
}

void paging_init()
{
  // TODO: Retrive the size of physical memory from multiboot header
  // Right now assume it to be 64MB
  unsigned int end_page_address = 0x4000000;
  frames_count = end_page_address / PAGE_SIZE;
  frames = (unsigned int*)kmalloc(INDEX_FROM_BIT(frames_count));
  memset(frames, 0, INDEX_FROM_BIT(frames_count));
  
  kernel_directory = (page_directory_t*) kmalloc_a(sizeof(page_directory_t));
  memset(kernel_directory, 0, sizeof(page_directory_t));

  for (unsigned int i = 0; i < placement_address; i += PAGE_SIZE) {
    alloc_frame(get_page(i, 1, kernel_directory), 1, 0);
  }

  switch_page_directory(kernel_directory);
  kheap = heap_create();
}

page_entry_t *get_page(unsigned int address, int should_make, page_directory_t *dir)
{
  address /= PAGE_SIZE;
  unsigned int table_index = address / 1024;

  if (dir->tables[table_index])
    return &dir->tables[table_index]->pages[address % 1024];

  if (!should_make)
    return 0;

  unsigned int tmp;
  dir->tables[table_index] = (page_table_t*) kmalloc_ap(sizeof(page_table_t), &tmp);
  memset(dir->tables[table_index], 0, sizeof(page_table_t));
  dir->tables_physical[table_index] = tmp | 0x7; // PRESENT, RW, USER;
  return &dir->tables[table_index]->pages[address % 1024];
}

void on_page_fault()
{
  lib_printf("PAGE FAULT\n");
  // TODO: REPLACE IT WITH PANIC
  while(1) {}
}

void* sbrk(unsigned int size)
{
  if (!size) {
    return (void*) placement_address;
  }
  unsigned int prev_top = placement_address;
  unsigned int addr = kmalloc_a(size);
  if (addr == (unsigned int) -1) {
    // TODO: put panic in place of this
    lib_printf("UNABLE TO ALLOC MEM\n");
    while(1) {}
  }
  for (unsigned int i = prev_top; i < placement_address; i += PAGE_SIZE) {
    alloc_frame(get_page(i, 1, kernel_directory), 1, 1);
  }
  return (void*) addr;
}
