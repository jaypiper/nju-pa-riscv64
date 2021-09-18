#include <cpu/exec.h>
#include "local-include/rtl.h"
#include <encoding.h>

#define IRQ_TIMER 0x8000000000000005

void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
#ifdef NANOS
  get_csr(SEPC_ID) = epc;
  get_csr(SCAUSE_ID) = NO;
  //将sstatus.SIE保存到sstatus.SPIE中, 然后将sstatus.SIE位置为0
  //sie: 1 spie: 5
  // printf("intr before: %lx ", get_csr(SSTATUS_ID));
  if(get_csr(SSTATUS_ID) & 2){
    get_csr(SSTATUS_ID) |= (1 << 5);
  }
  else{
    get_csr(SSTATUS_ID) &= ~(uintptr_t)(1 << 5);
  }
  get_csr(SSTATUS_ID) &= ~(uintptr_t)2;
  
  // printf("intr aft: %lx \n", get_csr(SSTATUS_ID));
  // printf("stvec %lx\n", get_csr(STVEC_ID));
  rtl_j(s, get_csr(STVEC_ID));
#else
  set_csr(CSR_MEPC, epc);
  set_csr(CSR_MCAUSE, NO);
  if(get_csr(CSR_MSTATUS) & (1 << 3)){
    set_csr(CSR_MSTATUS, get_csr(CSR_MSTATUS) | (1 << 7));
  }
  else{
    set_csr(CSR_MSTATUS, get_csr(MSTATUS_ID) & ~(uintptr_t)(1 << 7));
  }
  set_csr(CSR_MSTATUS, get_csr(MSTATUS_ID) & ~(uintptr_t)(1 << 3));
  rtl_j(s, get_csr(MTVEC_ID));
#endif
}

void query_intr(DecodeExecState *s) {
  // if(cpu.INTR && get_csr(SSTATUS_ID) != 0) printf("INTR: %d status: %lx %lx\n", cpu.INTR, get_csr(SSTATUS_ID), cpu.pc);
  if(cpu.INTR && get_csr(SSTATUS_ID) & 2){
    cpu.INTR = 0;
    raise_intr(s, IRQ_TIMER, cpu.pc);
    // printf("timer intr %lx jmp to %lx\n", cpu.pc, s->jmp_pc);
    update_pc(s);
  }
}
