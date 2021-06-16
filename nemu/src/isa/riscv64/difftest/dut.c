#include <isa.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"
#include "difftest.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  if(cpu.pc != ref_r->pc) return false;
  for(int i = 0; i < 32; i++){
    if(!difftest_check_reg(reg_name(i), pc, ref_r->gpr[i]._64, cpu.gpr[i]._64)) return false;
  }
  return true;
}

void isa_difftest_attach() {
}
