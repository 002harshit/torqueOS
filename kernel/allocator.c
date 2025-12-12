#include <arch/x86/paging.h>

#include <libcrank/std.h>

#include "./allocator.h"

block_t blocks[128];
unsigned int blocks_len = 0;
void allocator_init(struct multiboot_tag_mmap* mmap_tag, mem_region_t* used_regions, unsigned int used_regions_len)
{
  mem_region_t ram_regions[32];
  unsigned int ram_regions_len = 0;
  struct multiboot_mmap_entry* mmap = mmap_tag->entries;
  while ((char*) mmap < (char*) mmap_tag + mmap_tag->size) {
    if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
      ram_regions[ram_regions_len++] = (mem_region_t) {.start = mmap->addr, .end = mmap->addr + mmap->len};
    }
    mmap = (void*) ((unsigned long long) mmap + mmap_tag->entry_size);
  }

  printf("Len: %d\n", ram_regions_len);
  for (int i = 0; i < ram_regions_len; i++) {
    printf("region %lu -> %lu\n", ram_regions[i].start, ram_regions[i].end);
  }

  // assume used_regions does not overlap with each other
  for (unsigned int ui = 0; ui < used_regions_len; ui++) {
    for (unsigned int uj = ui+1; uj < used_regions_len; uj++) {
      if (!(used_regions[ui].end <= used_regions[uj].start || used_regions[uj].end <= used_regions[ui].start)) {
        printf("[ERROR] in allocator_init, used regions must not overlap each other\n");
        while(1) {}
        return;
      }
    }
  }
  for (int ui = 0; ui < used_regions_len; ui++) {
    unsigned long long paddr_start = used_regions[ui].start;
    unsigned long long paddr_end = used_regions[ui].end;
    blocks[blocks_len++] = (block_t) {.addr = used_regions[ui].start, .len = used_regions[ui].end - used_regions[ui].start, .is_used = 1};
    paging_map_region(paddr_start, paddr_end, 1, 1, 0);
  }

  for (int ri = 0; ri < ram_regions_len; ri++) {
    unsigned long long kaddr = ram_regions[ri].start;
    while (kaddr < ram_regions[ri].end && kaddr < MAX_32BIT_ADDRESS) {
      int uidx = -1;
      for (int ui = 0; ui < used_regions_len; ui++) {
        if (!(ram_regions[ri].end <= used_regions[ui].start || used_regions[ui].end <= kaddr)) {
          if (uidx == -1 || used_regions[ui].start < used_regions[uidx].start) {
            uidx = ui;
          }
        }
      }
      if (uidx == -1) {
        if (ram_regions[ri].end > MAX_32BIT_ADDRESS) {
          blocks[blocks_len++] = (block_t) {.addr = kaddr, .len = MAX_32BIT_ADDRESS - kaddr, .is_used = 0};
          kaddr = MAX_32BIT_ADDRESS;
        } else {    
          blocks[blocks_len++] = (block_t) {.addr = kaddr, .len = ram_regions[ri].end - kaddr, .is_used = 0};
          kaddr = ram_regions[ri].end;
        }
      } else if (used_regions[uidx].start <= kaddr) {
        kaddr = used_regions[uidx].end;
      } else {
        blocks[blocks_len++] = (block_t) {.addr = kaddr, .len = used_regions[uidx].start - kaddr, .is_used = 0};
        kaddr = used_regions[uidx].end;
      }
    }
  }
  printf("[INFO] INIT ALLCOATOR\n");
}
