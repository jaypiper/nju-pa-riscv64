#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FB_DISPINFO};

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len) ;

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]    = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT]   = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR]   = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_FB]       = {"/dev/fb", 0, 0, invalid_read, fb_write},
  [FD_EVENTS]   = {"/dev/events", 0, 0, events_read, invalid_write},
  [FB_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T _config = io_read(AM_GPU_CONFIG);
  assert(strcmp(file_table[FD_FB].name, "/dev/fb") == 0);
  file_table[FD_FB].size = _config.height * _config.width * 4;
}

size_t fs_read(int fd, void* buf, size_t len){
  // printf("fd: %d offset: %d\n", fd, file_table[fd].open_offset);
  int read_sz;
  if(file_table[fd].read == NULL){
    assert(file_table[fd].open_offset <= file_table[fd].size);
    read_sz = len + file_table[fd].open_offset <= file_table[fd].size ? len : file_table[fd].size - file_table[fd].open_offset;
    ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, read_sz);
  }
  else {
    read_sz = file_table[fd].read(buf, file_table[fd].open_offset, len);
  }
  file_table[fd].open_offset += read_sz;
  
  return read_sz;
}

size_t fs_write(int fd, const void* buf, size_t len){
  printf("fs write begin fd %d, buf: %lx, count %d\n", fd, (intptr_t)buf, len);
  int write_sz;
  if(file_table[fd].write == NULL){
    assert(file_table[fd].open_offset <= file_table[fd].size);
    write_sz = len + file_table[fd].open_offset <= file_table[fd].size ? len : file_table[fd].size - file_table[fd].open_offset;
    ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, write_sz);
  }
  else{
    write_sz = file_table[fd].write(buf, file_table[fd].open_offset, len);
  }
  printf("write phase1\n");
  file_table[fd].open_offset += write_sz;
  printf("fs write end\n");
  return write_sz;
}
//允许offset处于filesz外部，此时gap为'\0'
size_t fs_lseek(int fd, size_t offset, int whence){
  size_t _offset;
  switch(whence){
    case SEEK_SET: _offset = offset; break;
    case SEEK_CUR: _offset = file_table[fd].open_offset + offset; break;
    case SEEK_END: _offset = file_table[fd].size + offset; break;
    default: panic("Invalid lseek mode: %d\n", whence);
  }
  file_table[fd].open_offset = _offset;
  return _offset;
}

int fs_close(int fd){
  file_table[fd].open_offset = 0;
  return 0;
}

int fs_open(char* filename, int flags, int mode){
  // printf("opening %s", filename);
  int item_num = sizeof(file_table) / sizeof(Finfo);
  for(int i = 0; i < item_num; i++){
    // printf("%d %s %s %d\n", i, filename, file_table[i].name, strcmp(filename, file_table[i].name));
    if(strcmp(filename, file_table[i].name) == 0) {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  // panic("Invalid file: %s", filename);
  return -1;
}