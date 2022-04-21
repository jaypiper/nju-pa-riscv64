#ifndef NEMU_DISK_H
#define NEMU_DISK_H

#include <common.h>

#ifdef HAS_DISK
bool in_disk(paddr_t addr);
void load_kernel();
void* get_disk_addr(paddr_t addr);
void init_disk();
word_t disk_read(paddr_t addr, int len);
void disk_write(paddr_t addr, word_t data, int len);

#endif

#endif

