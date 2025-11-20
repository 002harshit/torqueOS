#include <libcrank/std.h>
#include <libcrank/string.h>

#include <kernel/ramfs.h>

#define PRINT_FILE_INFO(i) \
  printf("> file: ptr: %x, cap: %d, ",(unsigned int)fs.files[i].data, fs.files[i].capacity); \
  printf("siz: %d, idx: %d, name: %s\n", fs.files[i].size, i, fs.files[i].name)

void ramfs_test()
{
  printf("\n--- RAMFS test start\n");
  ramfs_t fs = ramfs_create(69);
  printf("> created ramfs: count: %d, ptr: %x\n", fs.count, (int)fs.files);
  int idx = ramfs_make_file(fs, "file1", 64);
  int idx2 = ramfs_make_file(fs, "file2", 400);

  int found = ramfs_get_index_from_name(fs, "file1");
  PRINT_FILE_INFO(idx);
  PRINT_FILE_INFO(found);
  int r1 = ramfs_is_file_valid(fs, idx);
  ramfs_delete_file(fs, idx);
  int r2 = ramfs_is_file_valid(fs, idx);
  printf("> valid before: %d, after: %d\n", r1, r2);

  int r3 = ramfs_write_string(fs, idx2, "ooga", 0);
  r3 = ramfs_write_string(fs, idx2, "ooga", 0);

  printf("> data: %s, written: %d\n", ramfs_get_data(fs, idx2), r3);
  printf("--- RAMFS test end\n");
}
