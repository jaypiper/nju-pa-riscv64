#include <isa.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"
#include "difftest.h"
#include <memory/paddr.h>
#include <monitor/monitor.h>

void init_serial();
void init_timer();
void init_vga();

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  if(cpu.pc != ref_r->pc) return false;
  for(int i = 0; i < 32; i++){
    if(!difftest_check_reg(reg_name(i), pc, ref_r->gpr[i]._64, cpu.gpr[i]._64)) return false;
  }
  return true;
}

void isa_difftest_attach() {
}

void isa_difftest_getregs(void* c){
  CPU_state* state = (CPU_state*)c; 
  for(int i = 0; i < 32; i++){
    state->gpr[i]._64 = cpu.gpr[i]._64;
  }
  state->pc = cpu.pc;
}

void isa_difftest_setregs(const void* c){
  CPU_state* state = (CPU_state*)c; 
  for(int i = 0; i < 32; i++){
    cpu.gpr[i]._64 = state->gpr[i]._64;
  }
  cpu.pc = state->pc;
}

void isa_difftest_memcpy_from_dut(paddr_t dest, void* src, size_t n){
  for(int i = 0; i < n; i++){
    paddr_write(dest + i, *(word_t*)(src+i), 1);
  }
}

void isa_difftest_init(){
  init_serial();
  init_timer();
  init_vga();
}
extern NEMUState nemu_state;

void is_nemu_trap(void* indi){
  if(nemu_state.state == NEMU_END) *(uint32_t*)indi = 1;
  else *(uint32_t*)indi = 0;
}