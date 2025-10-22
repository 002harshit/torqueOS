#ifndef PAGING_H
#define PAGING_H

#define PAGE_DIRECTORY_ENTRIES 1024
#define PAGE_TABLE_ENTRIES 1024
#define PAGE_SIZE 4096

typedef union page_entry {
  struct {
    unsigned int present:        1;
    unsigned int rw:             1;
    unsigned int user:           1;
    unsigned int pwt:            1;
    unsigned int pcd:            1;
    unsigned int accessed:       1;
    unsigned int dirty:          1;
    unsigned int pat:            1;
    unsigned int global:         1;
    unsigned int zero:           3;
    unsigned int frame:         20;
  } entry;
  unsigned int raw;
} page_entry_t;

typedef struct page_table {
  page_entry_t pages[PAGE_TABLE_ENTRIES];
} page_table_t;

typedef struct page_directory {
  page_table_t* tables[PAGE_DIRECTORY_ENTRIES];
  unsigned int tables_physical[PAGE_DIRECTORY_ENTRIES];
  unsigned int physical_address;
} page_directory_t;

void paging_init();
void on_page_fault();
void switch_current_directory(page_directory_t *dir);
page_entry_t *get_page(unsigned int address, int should_make, page_directory_t *dir);
void* sbrk(unsigned int size);

extern void load_page_directory(unsigned int* dir);

extern page_directory_t *current_directory;
extern page_directory_t *kernel_directory;

#endif // PAGING_H
