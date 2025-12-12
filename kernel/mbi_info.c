#include <libcrank/std.h>

#include "./mbi_info.h"

mbi_parsed_info_t parse_mbi(multiboot_info_t* mbi)
{
  printf("[INFO] MBI_SIZE: %d\n", mbi->total_size);

  // multiboot info contains array of tags ie terminated by tag.type = 0, tag.size = 8
  unsigned int tag_addr = (unsigned int) &mbi->tags;
  multiboot_tag_t* tag;
  struct multiboot_tag_framebuffer_common* fb_tag;
  struct multiboot_tag_mmap* mmap_tag;
  do {
    tag = (multiboot_tag_t*)(tag_addr);
    switch (tag->type) {
      case MULTIBOOT_TAG_TYPE_CMDLINE: {
        struct multiboot_tag_string* t = (void*) tag;
        printf("[INFO] CMDLINE ARGS: %s\n", t->string);
      } break;

      case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME: {
        struct multiboot_tag_string* t = (void*) tag;
        printf("[INFO] BOOT_LOADER_NAME: %s\n", t->string);
      } break;

      case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO: {
        struct multiboot_tag_basic_meminfo* t = (void*) tag;
        printf("[INFO] MEM_LOWER: %u, MEM_UPPER: %u\n", t->mem_lower, t->mem_upper);
      } break;

      case MULTIBOOT_TAG_TYPE_MMAP: {
        struct multiboot_tag_mmap *t = (void*) tag;
        mmap_tag = t;
      } break;

      case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
        struct multiboot_tag_framebuffer_common* t = (void*) tag;
        fb_tag = t;
        printf("[INFO] fb base: %lu\n", t->framebuffer_addr);
      } break;

      default: {
        printf("[WARN] MBI type {%d} not handled by parser\n", tag->type);
      }
    }
    tag_addr += MULTIBOOT_ALIGN_TAG(tag->size);
  } while (!(tag->type == 0 && tag->size == 8));

  unsigned int mbi_size = (unsigned int) tag_addr - (unsigned int)mbi;
  if (mbi_size != mbi->total_size) {
    printf("[ERROR] MBI SIZE CHECK FAILED: Expected <%d>  Got: <%d>\n", mbi->total_size, mbi_size);
    while(1) {}
  }

  if (!fb_tag || !mmap_tag) {
    printf("[ERROR] unable to find needed tags\n");
    while (1) {}
  }

  mbi_parsed_info_t parsed = {0};
  parsed.fb_tag = fb_tag;
  parsed.mmap_tag = mmap_tag;

  return parsed;
}


