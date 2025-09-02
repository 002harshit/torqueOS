#include "paging.h"

#define PAGE_DIRECTORY_SIZE 1024
#define PAGE_ENTRY_SIZE 1024
#define PAGE_FRAME_SIZE 4096

unsigned int page_directory[PAGE_DIRECTORY_SIZE] __attribute__((aligned(PAGE_FRAME_SIZE)));

unsigned int current_page[PAGE_ENTRY_SIZE] __attribute__((aligned(PAGE_FRAME_SIZE)));

void paging_init()
{
  // INFO: setting paging directory to 010 <Supervisor, Write Enabled, Not Present> state
  for (int i = 0; i < PAGE_DIRECTORY_SIZE; i++) {
    page_directory[i] = 0x2;
  }

  // INFO: setting this page to 011 <Supervisor, Write Enabled, Present> state
  for (int i = 0; i < PAGE_ENTRY_SIZE; i++) {
    current_page[i] = (i * PAGE_FRAME_SIZE) | 0x3;
  }

  page_directory[0] = ((unsigned int) current_page) | 0x3;

  set_paging(page_directory);



}



