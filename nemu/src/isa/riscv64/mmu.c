#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#include "local-include/reg.h"

paddr_t isa_mmu_translate(vaddr_t addr, int type, int len) {
  return MEM_RET_FAIL;
}

int isa_vaddr_check(vaddr_t vaddr, int type, int len){ //type好像也没什么用？ 或许是为了实现读写权限
  rtlreg_t _satp = reg_scr(SATP_ID);
  _satp >>= 60;
  switch(_satp){
    case 0: return MEM_RET_OK;
    case 1: return MEM_RET_NEED_TRANSLATE;
    default: assert(0); //return MEM_RET_FAIL;
  }
}