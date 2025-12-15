#include <libcrank/std.h>
#include <libcrank/string.h>

#include <kernel/ramfs.h>

#define PRINT_FILE_INFO(i) \
  printf("> file: ptr: %x, cap: %d, ",(unsigned int)fs.files[i].data, fs.files[i].capacity); \
  printf("siz: %d, idx: %d, name: %s\n", fs.files[i].size, i, fs.files[i].name)

extern char message_file_data;
extern char message_file_end;

void ramfs_test()
{
  printf("\n--- RAMFS test start\n");
  ramfs_t fs = ramfs_create(3);
  printf("> created ramfs: count: %d, ptr: %x\n", fs.count, (int)fs.files);
  int idx = ramfs_make_file(fs, "file1", 64);
  int idx2 = ramfs_make_file(fs, "file2", 400);

  int found = ramfs_get_index_from_name(fs, "file1");
  PRINT_FILE_INFO(idx);
  PRINT_FILE_INFO(idx2);
  PRINT_FILE_INFO(found);
  int r1 = ramfs_is_file_valid(fs, idx);
  ramfs_delete_file(fs, idx);
  int r2 = ramfs_is_file_valid(fs, idx);
  printf("> valid before: %d, after: %d\n", r1, r2);

  int r3 = ramfs_write_string(fs, idx2, "ooga", 0);
  r3 = ramfs_write_string(fs, idx2, "ooga", 0);

  printf("> data: %s, written: %d\n", ramfs_get_data(fs, idx2), r3);


  int message_size = (unsigned int)&message_file_end - (unsigned int)&message_file_data;
  char* message_data = &message_file_data;

  int file1 = ramfs_make_file(fs, "msg.txt", message_size);
  int written = ramfs_write_raw(fs, file1, message_data, message_size, 0);
  PRINT_FILE_INFO(file1);
  printf("> data: %s> written: %d\n", ramfs_get_data(fs, file1), written);
  
  char file_data[64];
  int file_size = 0;

  memcpy(file_data, ramfs_magic, sizeof(ramfs_magic));
  file_size+= sizeof(ramfs_magic);

  const char msg[] = "hello, worldo";
  strcpy(file_data + sizeof(ramfs_magic), msg);
  file_size+= sizeof(msg);

  int file2 = ramfs_insert(fs, "msg2.txt", file_data, file_size, sizeof(file_data));
  PRINT_FILE_INFO(file2);
  printf("> data: %s\n", ramfs_get_data(fs, file2));

  printf("--- RAMFS test end\n");
  ramfs_destroy(&fs);
}
