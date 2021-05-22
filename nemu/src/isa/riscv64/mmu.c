#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#include "local-include/reg.h"

#define VALID_MASK 1
#define PGTABLE_MASK 0x3ffffffffffc00
#define STAP_MASK 0xfffffffffff
#define PG_OFFSET 0xfff

paddr_t isa_mmu_translate(vaddr_t addr, int type, int len) {
  printf("here3\n");
  uint64_t* pg_base = (uint64_t*)((reg_scr(SATP_ID) & STAP_MASK) << 12);
  printf("base %lx\n", (uintptr_t)pg_base);
  uint64_t idx = (addr >> 30) & 0x1ff; 
  printf("idx\n"); 
  assert(pg_base[idx] & VALID_MASK);

  pg_base = (uint64_t*)pg_base[idx];
  idx = (addr >> 21) & 0x1ff;
  assert(pg_base[idx] & VALID_MASK);

  pg_base = (uint64_t*)pg_base[idx];
  idx = (addr >> 12) & 0x1ff;
  return (pg_base[idx] & PGTABLE_MASK) | (addr & PG_OFFSET);
}

word_t vaddr_mmu_read(vaddr_t addr, int len, int type){

  paddr_t paddr = isa_mmu_translate(addr, type, len);
  printf("paddr_read: %x \n", paddr);
  return paddr_read(paddr, len);

}

void vaddr_mmu_write(vaddr_t addr, word_t data, int len){
   printf("vaddr_write: %lx ", addr);
  paddr_t paddr = isa_mmu_translate(addr, 0, len);
  printf("paddr_write: %x \n", paddr);
  return paddr_write(paddr, data, len);
}

int isa_vaddr_check(vaddr_t vaddr, int type, int len){ //type好像也没什么用？ 或许是为了实现读写权限

  rtlreg_t _satp = reg_scr(SATP_ID);

  _satp >>= 60;

  switch(_satp){
    case 0: return MEM_RET_OK;
    case 8: return MEM_RET_NEED_TRANSLATE;
    default: assert(0); //return MEM_RET_FAIL;
  }
}
