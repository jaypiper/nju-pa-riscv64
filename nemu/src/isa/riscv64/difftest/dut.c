#include <isa.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"
// #include "difftest.h"
#include <memory/paddr.h>
#include <monitor/monitor.h>
#include <cpu/exec.h>
#include <encoding.h>

void init_serial();
void init_timer();
void init_vga();
void init_i8042();

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  if(cpu.pc != ref_r->pc) return false;
  for(int i = 0; i < 32; i++){
    if(!difftest_check_reg(reg_name(i), pc, ref_r->gpr[i], cpu.gpr[i])) return false;
  }
  for(int i = 0; i < csr_num; i++){
    int csr_id = csrs[i];
    if(!difftest_check_reg(csr_name[i], pc, ref_r->csr[csr_id], cpu.csr[csr_id])){
      return false;
    }
  }
  if(ref_r->privilege != cpu.privilege) return false;
  return true;
}

void isa_difftest_attach() {
}

static int diff_csrs[] = {
    SEPC_ID, STVEC_ID, SCAUSE_ID, STVAL_ID, SSCRATCH_ID, SSTATUS_ID,
    SATP_ID, SIE_ID, SIP_ID, MTVEC_ID, MEPC_ID, MCAUSE_ID, MIE_ID, MIP_ID, MTVAL_ID,
    MSCRATCH_ID, MSTATUS_ID, MHARTID, MEDELEG_ID, MIDELEG_ID
};

void isa_difftest_getregs(void* c){
  CPU_state* state = (CPU_state*)c; 
  for(int i = 0; i < 32; i++){
    state->gpr[i] = cpu.gpr[i];
  }
  for(int i = 0; i < sizeof(diff_csrs) / sizeof(int); i++){
    int id = diff_csrs[i];
    state->csr[id] = cpu.csr[id];
  }
  state->pc = cpu.pc;
  state->privilege = cpu.privilege;
  state->csr[CSR_USCRATCH] = cpu.csr[CSR_USCRATCH];
}

void isa_difftest_setregs(const void* c){
  CPU_state* state = (CPU_state*)c; 
  for(int i = 0; i < 32; i++){
    cpu.gpr[i] = state->gpr[i];
  }
  cpu.pc = state->pc;
  cpu.privilege = state->privilege;
  memcpy(cpu.csr, state->csr, sizeof(state->csr));
}


void isa_difftest_memcpy_from_dut(paddr_t dest, void* src, size_t n){
#ifdef FLASH
    void flash_init(paddr_t addr, word_t data, int len);
    for(int i = 0; i < n; i++){
      flash_init(dest + i, *(word_t*)(src+i), 1);
    }
#else
  for(int i = 0; i < n; i++){
    paddr_write(dest + i, *(word_t*)(src+i), 1);
  }
#endif
}

void isa_difftest_init(){
  init_serial();
  init_timer();
  init_vga();
  init_i8042();
  memset(cpu.csr, 0, sizeof(cpu.csr));
}

extern NEMUState nemu_state;

void is_nemu_trap(void* indi){
  if(nemu_state.state == NEMU_END || nemu_state.state == NEMU_ABORT) *(uint32_t*)indi = 1;
  else *(uint32_t*)indi = 0;
}

#define IRQ_TIMER 0x8000000000000007
// void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc);

void isa_raise_intr(){
  // DecodeExecState s;
  // raise_intr(&s, IRQ_TIMER, cpu.pc);
  // update_pc(&s);
}