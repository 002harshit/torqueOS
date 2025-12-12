#ifndef KERNEL_MBI_INFO_H
#define KERNEL_MBI_INFO_H

#include "./multiboot2.h"

typedef struct {
  unsigned long long start;
  unsigned long long end;
} ram_region_t;

typedef struct {
  struct multiboot_tag_framebuffer_common* fb_tag;
  struct multiboot_tag_mmap* mmap_tag;
} mbi_parsed_info_t;

mbi_parsed_info_t parse_mbi(multiboot_info_t* mbi);

#endif // KERNEL_MBI_INFO_H
