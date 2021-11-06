#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#include "local-include/reg.h"
#include "local-include/mmu.h"
#include <cpu/decode.h>
#include <csr.h>

#define VALID_MASK 1
#define PGTABLE_MASK 0x3ffffffffffc00LL
#define STAP_MASK 0xfffffffffffLL
#define PG_OFFSET 0xfff
#define SV39_PREFIX_MASK 0x1ffffff
paddr_t isa_mmu_translate(DecodeExecState *s, vaddr_t addr, int type, int len) {
  int levels = 3;
  uint64_t prefix = addr >> 39;
  uint64_t msb = (addr >> 38) & 1;
  if(!(((prefix & SV39_PREFIX_MASK) == 0 &&(!msb)) || ((prefix & SV39_PREFIX_MASK) == SV39_PREFIX_MASK && msb))){
    printf("above addr: %lx\n", addr);
    levels = 0; // bits 63–39 all equal to bit 38, or else page fault
  }
  uint64_t ppn = get_csr(SATP_ID) & STAP_MASK;
  uint64_t* pg_base = (uintptr_t*)(ppn << 12);
  uint64_t shift_num = 30;
  uint64_t sum = get_csr(CSR_MSTATUS) & MSTATUS_SUM;
  uint64_t mxr = get_csr(CSR_MSTATUS) & MSTATUS_MXR;
  uint64_t ad = PTE_A | ((type == MEM_TYPE_WRITE) * PTE_D);
  for(int i = levels-1; i >= 0; i--){
    uint64_t idx = (addr >> shift_num) & 0x1ff;
    uintptr_t pte = paddr_read(s, (uintptr_t)(pg_base + idx), sizeof(uintptr_t), MEM_TYPE_READ); // misaligned will never occur
    if(s->is_trap) return 0;
    ppn = (pte & PGTABLE_MASK) >> 10;
    if(PTE_TABLE(pte)){ //pointer to next level of page table
      if(pte & (PTE_D | PTE_A | PTE_U)) break;
      pg_base = (uintptr_t*)((pte & PGTABLE_MASK) << 2);
    }else if((pte & PTE_U) ? (get_priv() == PRV_S && (!sum || type == MEM_TYPE_IFETCH)) : get_priv() == PRV_U){
      // U mode software can only access(U=1); sum=1 S-mode can access(but can not execute)
      break;
    }else if(!(pte & PTE_V) || (!(pte & PTE_R) && (pte & PTE_W))){
      break;
    }else if((type == MEM_TYPE_IFETCH && !(pte & PTE_X)) ||
             (type == MEM_TYPE_READ && !((pte & PTE_R) || (mxr && (pte & PTE_X)))) ||
             (type == MEM_TYPE_WRITE && !(pte & PTE_W))){
      break;
    }else{ // leaf pte here
      if((pte & ad) != ad){
        paddr_write(s, (uintptr_t)(pg_base + idx), pte | ad, sizeof(uintptr_t), MEM_TYPE_WRITE);
      }
      // super-page
      if(i > 0 && (ppn & ((1 << (9 * i)) - 1)) != 0) break;
      uint64_t pg_offset = addr & ((1 << (shift_num))-1);
      return ((pte & PGTABLE_MASK) << 2)  | pg_offset;
    }

    shift_num -= 9;
  }

  s->is_trap = true;
  s->trap.tval = addr;
  switch(type){
    case MEM_TYPE_IFETCH:
        s->trap.cause = CAUSE_FETCH_PAGE_FAULT; break;
    case MEM_TYPE_READ:
        s->trap.cause = CAUSE_LOAD_PAGE_FAULT; break;
    case MEM_TYPE_WRITE:
        s->trap.cause = CAUSE_STORE_PAGE_FAULT; break;
    default:
      assert(0);
  }
  return 0;
}

word_t vaddr_mmu_read(DecodeExecState *s, vaddr_t addr, int len, int type){
  paddr_t paddr = isa_mmu_translate(s, addr, type, len);
  if(s->is_trap) {
    s->trap.tval = addr;
    return 0;
  }
  if((paddr & (PAGE_SIZE - 1)) + len > PAGE_SIZE && (type == MEM_TYPE_IFETCH)){
    int first = PAGE_SIZE - (paddr & (PAGE_SIZE-1));
    uint32_t inst1 = paddr_read(s, paddr, first, type);
    if((inst1 & 3) != 3){
      return paddr_read(s, paddr, first, type);
    }
    paddr_t paddr2 = isa_mmu_translate(s, addr + first, type, len - first);
    if(s->is_trap) {
      s->trap.tval = addr + first;
      return 0;
    }
    return paddr_read(s, paddr2, len - first, type) << (8 * first) | paddr_read(s, paddr, first, type);
  }else{
    return paddr_read(s, paddr, len, type);
  }

}

void vaddr_mmu_write(DecodeExecState *s, vaddr_t addr, word_t data, int len){
  paddr_t paddr = isa_mmu_translate(s, addr, MEM_TYPE_WRITE, len);
  if(s->is_trap) return;
  return paddr_write(s, paddr, data, len, MEM_TYPE_WRITE);
}

#ifdef VME
int isa_vaddr_check(vaddr_t vaddr, int type, int len){
  int mode = cpu.privilege;
  if(type != MEM_TYPE_IFETCH){
    if((get_csr(CSR_MSTATUS) & MSTATUS_MPRV) != 0){
      mode = get_val(get_csr(CSR_MSTATUS), MSTATUS_MPP);
    }
  }
  if(mode == PRV_M) return MEM_RET_OK;

  rtlreg_t _satp = get_csr(SATP_ID);

  _satp >>= 60;

  switch(_satp){
    case 0: return MEM_RET_OK;
    case 8: return MEM_RET_NEED_TRANSLATE;
    default: assert(0); //return MEM_RET_FAIL;
  }
}
#endif

paddr_t vaddr_translate(DecodeExecState* s, vaddr_t vaddr, int type, int len){
  int ret = isa_vaddr_check(vaddr, type, len);
  if(ret == MEM_RET_OK) return vaddr;
  return isa_mmu_translate(s, vaddr, type, len);
}