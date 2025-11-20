#ifndef RAMFS_H
#define RAMFS_H

#define MAX_FILENAME_SIZE 32
#define RAMFS_FILE_MAGIC {'R','A','M','F','S','7','0','S'} /* just some random bs value */

typedef struct {
  // right now file capacity should not be changed once allocated
  // as im not trying to manage memory using some fancy allocators
  // i cant really realloc the file_data based on size
  unsigned int capacity;

  // size = file_size + magic_size
  unsigned int size;

  // the first 8 bytes must be file magic
  char* data;

  char name[MAX_FILENAME_SIZE];

  // TODO: add and manage info for file created & modified date, read,write perms etc
} ramfs_file_t;

typedef struct {
  ramfs_file_t* files;
  const unsigned int count;
} ramfs_t;

ramfs_t ramfs_create(unsigned int count);
int ramfs_get_index_from_name(const ramfs_t fs, const char* name);
int ramfs_make_file(const ramfs_t fs, const char* name, unsigned int max_size);
void ramfs_delete_file(const ramfs_t fs, int index);
int ramfs_is_file_valid(const ramfs_t fs, int index);
char* ramfs_get_data(const ramfs_t fs, int index);

// data must be null terminated string
unsigned int ramfs_write_string(const ramfs_t fs, int index, const char* data, int should_append);

#endif // RAMFS_H
