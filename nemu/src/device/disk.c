#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#include <device/map.h>
#include <stdlib.h>
#include <time.h>
#include <encoding.h>

#ifdef HAS_DISK

#define DISK_START 0x40000000
#define DISK_SIZE (128 * MB)
static uint8_t disk[DISK_SIZE] PG_ALIGN = {};

bool in_disk(paddr_t addr){
  return (DISK_START <= addr) && (addr < DISK_START + DISK_SIZE);
}

void load_kernel(){
  memcpy(guest_to_host(0), get_disk_addr(0), 1 * MB);
}

void* get_disk_addr(paddr_t addr){
  return &disk[addr];
}

void init_disk(){

}

word_t disk_read(paddr_t addr, int len){
  void *p = &disk[addr - DISK_START];
  switch(len){
    case 1: return *(uint8_t*)p;
    case 2: return *(uint16_t *)p;
    case 4: return *(uint32_t *)p;
    case 8: return *(uint64_t *)p;
  }
  assert(0);
}

void disk_write(paddr_t addr, word_t data, int len){
  void *p = &disk[addr - DISK_START];
  switch (len) {
    case 1: *(uint8_t  *)p = data; return;
    case 2: *(uint16_t *)p = data; return;
    case 4: *(uint32_t *)p = data; return;
    case 8: *(uint64_t *)p = data; return;
  }
  assert(0);
}


#endif