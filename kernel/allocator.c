#include <arch/x86/paging.h>

#include <libcrank/std.h>

#include "./allocator.h"
mem_allocator_t _kallocator;

static inline int find_best_fit(mem_allocator_t* allo, unsigned int size)
{
  int best = -1;
  for (unsigned int b = 0; b < allo->blocks_len; b++) {
    if (allo->blocks[b].state != MEM_BLOCK_FREE) {
      continue;
    }
    if (allo->blocks[b].len >= size) {
      if (best == -1 || allo->blocks[b].len < allo->blocks[best].len) {
        best = b;
      }
    }
  }
  return best;
}

static inline void create_region_pages(mem_allocator_t* allo, int create_index)
{
  mem_block_t cblock = allo->blocks[create_index];
  paging_map_region(cblock.addr, cblock.addr + cblock.len, 1, 1);
}

static void* malloc_unsafe(mem_allocator_t* allo, unsigned int size)
{
  int best = find_best_fit(allo, size);
  if (best < 0) {
    return NULL;
  }

  unsigned long long splitted_len = allo->blocks[best].len;
  while (splitted_len / 2 > size) {
    splitted_len /= 2;
  }

  mem_block_t block_a = {.addr = allo->blocks[best].addr, .len = allo->blocks[best].len - splitted_len, .state = MEM_BLOCK_FREE};
  mem_block_t block_b = {.addr = block_a.addr + block_a.len, .len = splitted_len, .state = MEM_BLOCK_USED};
  if (block_a.len == 0) {
    allo->blocks[best] = block_b;
  } else {
    allo->blocks[allo->blocks_len++] = block_a;
    allo->blocks[best] = block_b;
  }
  create_region_pages(allo, best);
  return (void*)block_b.addr;
}

static inline int find_block_from_ptr(mem_allocator_t* allo, void* ptr)
{
  for (unsigned int b = 0; b < allo->blocks_len; b++) {
    if (allo->blocks[b].addr == (unsigned int) ptr) {
      if (allo->blocks[b].state == MEM_BLOCK_RESTRICTED)
        return -1;
      return b;
    }
  }
  return -1;
}

static inline void free_region_pages(mem_allocator_t* allo, int freed_index)
{
  mem_block_t fblock = _kallocator.blocks[freed_index];
  unsigned int paddr_start = fblock.addr;
  unsigned int paddr_end = fblock.addr + fblock.len;

  // INFO: checks whether neighbour boundary is unused as well, it they are then page align according to it
  for (int i = allo->blocks_len - 1; i > -1; i--) {
    if (freed_index == i) {
      continue;
    }
    mem_block_t block = allo->blocks[i];
    if (fblock.addr == block.addr + block.len && block.state == MEM_BLOCK_FREE) {
      unsigned int new_addr = (paddr_start) & ~(FRAME_SIZE - 1);
      if (new_addr >= block.addr) {
        paddr_start  = new_addr;
      }
    }
    if (fblock.addr + fblock.len == block.addr && block.state == MEM_BLOCK_FREE) {
      unsigned int new_addr = (paddr_end + FRAME_SIZE - 1) & ~(FRAME_SIZE - 1);
      if (new_addr <= block.addr + block.len) {
        paddr_end = new_addr;
      }
    }
  }

  for (unsigned int addr = paddr_start; addr + FRAME_SIZE < paddr_end; addr += FRAME_SIZE) {
    if (addr + FRAME_SIZE < paddr_end)
      paging_map_page(addr, 0, 0);
  }
}

static int free_unsafe(mem_allocator_t* allo, void* ptr)
{
  int bidx = find_block_from_ptr(allo, ptr);
  if (bidx > -1) {
    if (allo->blocks[bidx].state != MEM_BLOCK_FREE) {
      allo->blocks[bidx].state = MEM_BLOCK_FREE;
      free_region_pages(allo, bidx);
      return bidx;
    }
  }
  return -1;
}

static void* realloc_unsafe(mem_allocator_t* allo, void* ptr, unsigned int size)
{
  int bidx = find_block_from_ptr(allo, ptr);
  if (bidx < 0) {
    return NULL;
  }
  if (!allo->blocks[bidx].state != MEM_BLOCK_FREE) {
    return NULL;
  }
  if (allo->blocks[bidx].len > size) {
    return (void*) allo->blocks[bidx].addr;
  }

  unsigned char* old_data = (unsigned char*) allo->blocks[bidx].addr;
  unsigned char* new_data = (unsigned char*) malloc_unsafe(allo, size);
  if (!new_data) {
    return NULL;
  }

  for (unsigned long long i = 0; i < allo->blocks[bidx].len; i++) {
    new_data[i] = old_data[i];
  }

  free_unsafe(allo, old_data);

  return new_data;
}

static void allocator_extend_bounds(mem_allocator_t* allo)
{
  if (allo->blocks_len + 4 > allo->blocks_cap) {
    unsigned int new_capacity = allo->blocks_cap * 2;
    mem_block_t* new_blocks = malloc_unsafe(allo, sizeof(mem_block_t) * new_capacity);
    for (unsigned int b = 0; b < allo->blocks_len; b++) {
      new_blocks[b] = allo->blocks[b];
    }
    free_unsafe(allo, allo->blocks);
    allo->blocks_cap = new_capacity;
    allo->blocks = new_blocks;
  }
}

void* allocator_malloc(unsigned int size)
{
  allocator_extend_bounds(&_kallocator);
  void* ptr = malloc_unsafe(&_kallocator, size);
  return ptr;
}

int allocator_free(void* ptr)
{
  int freed_index = free_unsafe(&_kallocator, ptr);
  if (freed_index < 0) return 0;
  // TODO: merge adjacent !is_used blocks

  return 1;
}

void* allocator_realloc(void* old_ptr, unsigned int new_size)
{
  allocator_extend_bounds(&_kallocator);
  return realloc_unsafe(&_kallocator, old_ptr, new_size);
}

void allocator_init(struct multiboot_tag_mmap* mmap_tag, mem_region_t* used_regions, unsigned int used_regions_len)
{
  const unsigned int blocks_cap = 128;
  mem_block_t blocks[blocks_cap];
  unsigned int blocks_len = 0;
 
  mem_region_t ram_regions[32];
  unsigned int ram_regions_len = 0;
  struct multiboot_mmap_entry* mmap = mmap_tag->entries;
  while ((char*) mmap < (char*) mmap_tag + mmap_tag->size) {
    if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
      ram_regions[ram_regions_len++] = (mem_region_t) {.start = mmap->addr, .end = mmap->addr + mmap->len};
    }
    mmap = (void*) ((unsigned int) mmap + mmap_tag->entry_size);
  }

  printf("[INFO] ram region len: %d\n", ram_regions_len);
  for (unsigned int i = 0; i < ram_regions_len; i++) {
    printf("> ram_region[%d] %lu -> %lu\n", i, ram_regions[i].start, ram_regions[i].end);
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
  for (unsigned int ui = 0; ui < used_regions_len; ui++) {
    unsigned long long paddr_start = used_regions[ui].start;
    unsigned long long paddr_end = used_regions[ui].end;
    blocks[blocks_len++] = (mem_block_t) {.addr = used_regions[ui].start, .len = used_regions[ui].end - used_regions[ui].start, .state = MEM_BLOCK_RESTRICTED};
    paging_map_region(paddr_start, paddr_end, 1, 1);
  }

  for (unsigned int ri = 0; ri < ram_regions_len; ri++) {
    unsigned long long kaddr = ram_regions[ri].start;
    while (kaddr < ram_regions[ri].end && kaddr < MAX_32BIT_ADDRESS) {
      int uidx = -1;
      for (unsigned int ui = 0; ui < used_regions_len; ui++) {
        if (!(ram_regions[ri].end <= used_regions[ui].start || used_regions[ui].end <= kaddr)) {
          if (uidx == -1 || used_regions[ui].start < used_regions[uidx].start) {
            uidx = ui;
          }
        }
      }
      if (uidx == -1) {
        if (ram_regions[ri].end > MAX_32BIT_ADDRESS) {
          blocks[blocks_len++] = (mem_block_t) {.addr = kaddr, .len = MAX_32BIT_ADDRESS - kaddr, .state = MEM_BLOCK_FREE};
          kaddr = MAX_32BIT_ADDRESS;
        } else {    
          blocks[blocks_len++] = (mem_block_t) {.addr = kaddr, .len = ram_regions[ri].end - kaddr, .state = MEM_BLOCK_FREE};
          kaddr = ram_regions[ri].end;
        }
      } else if (used_regions[uidx].start <= kaddr) {
        kaddr = used_regions[uidx].end;
      } else {
        blocks[blocks_len++] = (mem_block_t) {.addr = kaddr, .len = (used_regions[uidx].start - kaddr), .state = MEM_BLOCK_FREE};
        kaddr = used_regions[uidx].end;
      }
    }
  }

  mem_allocator_t tmp_allo = {.blocks = &blocks[0], .blocks_cap = blocks_cap, .blocks_len = blocks_len};

  mem_block_t* new_blocks = malloc_unsafe(&tmp_allo, blocks_cap * sizeof(mem_block_t));
  for (unsigned int i = 0; i < blocks_len; i++) {
    new_blocks[i] = blocks[i];
  }

  _kallocator.blocks = new_blocks;
  _kallocator.blocks_len = blocks_len;
  _kallocator.blocks_cap = blocks_cap;

  printf("[INFO] Initialized allocator\n");
}
