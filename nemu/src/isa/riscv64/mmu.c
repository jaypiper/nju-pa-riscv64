#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#include "local-include/reg.h"
#include "local-include/mmu.h"
#include <cpu/decode.h>

#define VALID_MASK 1
#define PGTABLE_MASK 0x3ffffffffffc00LL
#define STAP_MASK 0xfffffffffffLL
#define PG_OFFSET 0xfff
#define SV39_PREFIX_MASK 0x1ffffff
paddr_t isa_mmu_translate(DecodeExecState *s, vaddr_t addr, int type, int len) {
  uint64_t* pg_base = (uintptr_t*)((get_csr(SATP_ID) & STAP_MASK) << 12);
  uint64_t prefix = addr >> 39;
  uint64_t msb = (addr >> 38) & 1;
  if(!(((prefix & SV39_PREFIX_MASK) == 0 &&(!msb)) || ((prefix & SV39_PREFIX_MASK) == SV39_PREFIX_MASK && msb))){
    s->is_trap = true;
    return 0;
  }

  uint64_t idx = (addr >> 30) & 0x1ff;
  uintptr_t val = paddr_read((uintptr_t)(pg_base + idx), sizeof(uintptr_t));
  if(!(val&VALID_MASK)){
    s->is_trap = true;
    return 0;
  }

  pg_base = (uintptr_t*)((val & PGTABLE_MASK) << 2);
  idx = (addr >> 21) & 0x1ff;
  val = paddr_read((uintptr_t)(pg_base + idx), sizeof(uintptr_t));
  if(!(val&VALID_MASK)){
    s->is_trap = true;
    return 0;
  }

  pg_base = (uintptr_t*)((val & PGTABLE_MASK) << 2);
  idx = (addr >> 12) & 0x1ff;
  val = paddr_read((uintptr_t)(pg_base + idx), sizeof(uintptr_t));
  if(!(val&VALID_MASK)){
    s->is_trap = true;
    return 0;
  }
  return ((val & PGTABLE_MASK) << 2)  | (addr & PG_OFFSET);
}

word_t vaddr_mmu_read(DecodeExecState *s, vaddr_t addr, int len, int type){
  paddr_t paddr = isa_mmu_translate(s, addr, type, len);
  if(s->is_trap) return 0;
  if((paddr & (PAGE_SIZE - 1)) + len > PAGE_SIZE){
    printf("paddr_read: addr: %lx paddr: %x len: %d\n", addr, paddr, len);
    assert(0);
  }
  return paddr_read(paddr, len);

}

void vaddr_mmu_write(DecodeExecState *s, vaddr_t addr, word_t data, int len){
  paddr_t paddr = isa_mmu_translate(s, addr, 0, len);
  if(s->is_trap) return;
  return paddr_write(paddr, data, len);
}

#ifdef VME
int isa_vaddr_check(vaddr_t vaddr, int type, int len){ //type好像也没什么用？ 或许是为了实现读写权限
  if(cpu.privilege == PRV_M) return MEM_RET_OK;
  rtlreg_t _satp = get_csr(SATP_ID);

  _satp >>= 60;

  switch(_satp){
    case 0: return MEM_RET_OK;
    case 8: return MEM_RET_NEED_TRANSLATE;
    default: assert(0); //return MEM_RET_FAIL;
  }
}
#endif
