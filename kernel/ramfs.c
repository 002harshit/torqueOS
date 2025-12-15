#include "./ramfs.h"
#include "./allocator.h"

#include <libcrank/string.h>

// TODO: fix memory leaks, and change interface of ramfs

ramfs_t ramfs_create(unsigned int count)
{
  if (count < 1) {
    return (ramfs_t) {.files = (void*) 0, .count = 0};
  }
  ramfs_file_t* files = allocator_malloc(sizeof(ramfs_file_t) * count);
  for (unsigned int i = 0; i < count; i++) {
    files[i].capacity = 0;
    files[i].size = 0;
    files[i].data = (void*) 0;
    files[i].name[0] = '\0';
  }
  return (ramfs_t) {.files = files, .count = count};
}

void ramfs_destroy(ramfs_t* fs)
{
  for (unsigned int i = 0; i < fs->count; i++) {
    if (fs->files[i].capacity)
      allocator_free(fs->files[i].data);
  }
  allocator_free(fs->files);
  fs->files = 0;
  fs->count = 0;
}

int ramfs_get_index_from_name(const ramfs_t fs, const char* name)
{
  for (unsigned int i = 0; i < fs.count; i++) {
    ramfs_file_t file = fs.files[i];
    if (!ramfs_is_file_valid(fs, i) || strncmp(name, file.name, MAX_FILENAME_SIZE) != 0) {
      continue;
    }
    return i;
  }
  return -1;
}

static int find_best_fit(const ramfs_t fs, unsigned int capacity)
{
  int best_fit = -1;
  unsigned int best_capacity = 0;
  for (unsigned int i = 0; i < fs.count; i++) {
    if (ramfs_is_file_valid(fs, i)) {
      continue;
    }
    ramfs_file_t file = fs.files[i];
    if (file.capacity < capacity) {
      continue;
    }
    if (best_fit < 0 || best_capacity > file.capacity) {
      best_fit = i;
      best_capacity = file.capacity;
    }
  }
  return best_fit;
}

static int alloc_file(const ramfs_t fs, unsigned int capacity)
{
  for (unsigned int i = 0; i < fs.count; i++) {
    if (ramfs_is_file_valid(fs, i)) {
      continue;
    }
    ramfs_file_t file = fs.files[i];
    if (file.capacity != 0) {
      continue;
    }
    fs.files[i].data = allocator_malloc(capacity);
    return i;
  }
  return -1;
}

// ramfs_make_file, like Makefile?? haha
int ramfs_make_file(const ramfs_t fs, const char* name, unsigned int max_size)
{
  // BUG: currently it may support duplicate file names

  // INFO: as im keeping track of file size 
  // null terminator might be redundent in this case
  // but would be very handle if user just wanted to print whole data as string
  unsigned int capacity = max_size + sizeof(ramfs_magic) + 1; // added 1 cuz of null terminator

  int best_fit = find_best_fit(fs, capacity);

   // if unable to allocated
  if (best_fit < 0) {
    best_fit = alloc_file(fs, capacity);
    if (best_fit < 0) {
      return -1;
    }
  }

  strncpy(fs.files[best_fit].name, name, MAX_FILENAME_SIZE - 1);
  fs.files[best_fit].capacity = capacity;
  fs.files[best_fit].size = 0;
  for (unsigned int i = 0; i < sizeof(ramfs_magic); i++) {
    fs.files[best_fit].data[i] = ramfs_magic[i];
    fs.files[best_fit].size++;
  }

  fs.files[best_fit].data[fs.files[best_fit].size] = '\0';

  return best_fit;
}

void ramfs_delete_file(const ramfs_t fs, int index)
{
  if (index >= (int) fs.count || index < 0) {
    return;
  }
  fs.files[index].size = 0;
  fs.files[index].name[0] = '\0';
}

int ramfs_is_file_valid(const ramfs_t fs, int index)
{
  if (index >= (int) fs.count || index < 0) {
    return 0;
  }
  ramfs_file_t file = fs.files[index];
  if (file.capacity <= sizeof(ramfs_magic) || file.size < sizeof(ramfs_magic)|| file.name[0] == '\0') {
    return 0;
  }
  for (unsigned int i = 0; i < sizeof(ramfs_magic); i++) {
    if (file.data[i] != ramfs_magic[i]) {
      return 0;
    }
 }
  return 1;
}

int ramfs_insert(const ramfs_t fs, const char* file_name, char* file_data, unsigned int file_size, unsigned int file_cap)
{
  if (file_size >= file_cap) {
    file_cap = file_size;
  }

  ramfs_file_t tmp_file = {
    .data = file_data,
    .capacity = file_cap,
    .size = file_size,
  };
  strncpy(tmp_file.name, file_name, MAX_FILENAME_SIZE);
  ramfs_t temp_fs = {.files = &tmp_file, .count = 1};
  if (!ramfs_is_file_valid(temp_fs, 0)) {
    return -1;
  }

  for (unsigned int i = 0; i < fs.count; i++) {
    if (ramfs_is_file_valid(fs, i)) {
      continue;
    }

    ramfs_file_t file = fs.files[i];
    if (file.capacity != 0) {
      continue;
    }

    fs.files[i].capacity = file_cap;
    fs.files[i].size = file_size;
    fs.files[i].data = file_data;
    strncpy(fs.files[i].name, file_name, MAX_FILENAME_SIZE);
    return i;
  }
  return -1;
}

char* ramfs_get_data(const ramfs_t fs, int index)
{
  if (!ramfs_is_file_valid(fs, index)) {
    return NULL;
  }
  return fs.files[index].data + sizeof(ramfs_magic);
}

unsigned int ramfs_write_raw(const ramfs_t fs, int index, const char* data, unsigned int data_size, int should_append)
{
  if (!ramfs_is_file_valid(fs, index) || !data_size) {
    return 0;
  }
  ramfs_file_t f = fs.files[index];
  if (!should_append) {
    f.size = sizeof(ramfs_magic);
  }

  unsigned int count = 0;
  for (unsigned int i = 0; i < data_size; i++) {
    if (f.size + 1 >= f.capacity) {
      break;
    }
    f.data[f.size++] = data[i];
    count++;
  }
  f.data[f.size] = '\0';
  fs.files[index] = f;
  return count;
}

unsigned int ramfs_write_string(const ramfs_t fs, int index, const char* data, int should_append)
{
  unsigned int data_size = strlen(data);
  ramfs_write_raw(fs, index, data, data_size, should_append);
}
