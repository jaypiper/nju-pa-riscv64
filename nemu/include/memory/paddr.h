#ifndef __MEMORY_PADDR_H__
#define __MEMORY_PADDR_H__

#include <common.h>
#include <device/disk.h>

#define PMEM_SIZE (128 * 1024 * 1024)

/* convert the guest physical address in the guest program to host virtual address in NEMU */
void* guest_to_host(paddr_t addr);
/* convert the host virtual address in NEMU to guest physical address in the guest program */
paddr_t host_to_guest(void *addr);

#ifdef FLASH
void* get_flash_addr(paddr_t addr);
#endif

word_t paddr_read(DecodeExecState* s, paddr_t addr, int len, int type);
void paddr_write(DecodeExecState* s, paddr_t addr, word_t data, int len, int type);

#endif
