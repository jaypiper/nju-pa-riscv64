#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#include "local-include/reg.h"

#define VALID_MASK 1
#define PGTABLE_MASK 0x3ffffffffffc00LL
#define STAP_MASK 0xfffffffffffLL
#define PG_OFFSET 0xfff

paddr_t isa_mmu_translate(vaddr_t addr, int type, int len) {
  uint64_t* pg_base = (uintptr_t*)((reg_scr(SATP_ID) & STAP_MASK) << 12);
  uint64_t idx = (addr >> 30) & 0x1ff; 
  
  // printf("base %lx idx: %ld addr: %lx\n", (uintptr_t)pg_base, idx, addr);

  uintptr_t val = paddr_read((uintptr_t)(pg_base + idx), sizeof(uintptr_t));
  if(!(val&VALID_MASK)){
    printf("base %lx idx: %ld addr: %lx pc: %lx %d\n", (uintptr_t)pg_base, idx, addr, cpu.pc, type);
    printf("val1: %lx\n", val); 
  }
  assert(val & VALID_MASK);

  pg_base = (uintptr_t*)((val & PGTABLE_MASK) << 2);
  idx = (addr >> 21) & 0x1ff;
  val = paddr_read((uintptr_t)(pg_base + idx), sizeof(uintptr_t));
  // printf("val2: %lx\n", val);
  if(!(val&VALID_MASK)){
    printf("base %lx idx: %ld addr: %lx pc: %lx %d\n", (uintptr_t)pg_base, idx, addr, cpu.pc, type);
    printf("val2: %lx\n", val); 
  }
  assert(val & VALID_MASK);

  pg_base = (uintptr_t*)((val & PGTABLE_MASK) << 2);
  idx = (addr >> 12) & 0x1ff;
  val = paddr_read((uintptr_t)(pg_base + idx), sizeof(uintptr_t));
  // printf("val3: %lx\n", val);
  if(!(val&VALID_MASK)){
    printf("base %lx idx: %ld addr: %lx pc: %lx %d\n", (uintptr_t)pg_base, idx, addr, cpu.pc, type);
    printf("val3: %lx\n\n", val); 
  }
  assert(val & VALID_MASK);
  
  return ((val & PGTABLE_MASK) << 2)  | (addr & PG_OFFSET);
}

word_t vaddr_mmu_read(vaddr_t addr, int len, int type){
  paddr_t paddr = isa_mmu_translate(addr, type, len);
  if((paddr & (PAGE_SIZE - 1)) + len > PAGE_SIZE){
    printf("paddr_read: addr: %lx paddr: %x len: %d\n", addr, paddr, len);
    assert(0);
  }
  // if(addr < 0x80000000) printf("paddr_read: addr: %lx paddr: %x len: %d\n", addr, paddr, len);
  return paddr_read(paddr, len);

}

void vaddr_mmu_write(vaddr_t addr, word_t data, int len){
  //  printf("vaddr_write: %lx ", addr);
  paddr_t paddr = isa_mmu_translate(addr, 0, len);
  // printf("paddr_write: %x \n", paddr);
  return paddr_write(paddr, data, len);
}

#ifdef VME
int isa_vaddr_check(vaddr_t vaddr, int type, int len){ //type好像也没什么用？ 或许是为了实现读写权限

  rtlreg_t _satp = reg_scr(SATP_ID);

  _satp >>= 60;

  switch(_satp){
    case 0: return MEM_RET_OK;
    case 8: return MEM_RET_NEED_TRANSLATE;
    default: assert(0); //return MEM_RET_FAIL;
  }
}
#endif
