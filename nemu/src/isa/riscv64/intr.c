#include <cpu/exec.h>
#include "local-include/rtl.h"
#define IRQ_TIMER 0x8000000000000005

void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  reg_scr(SEPC_ID) = epc;
  reg_scr(SCAUSE_ID) = NO;
  //将sstatus.SIE保存到sstatus.SPIE中, 然后将sstatus.SIE位置为0
  //sie: 1 spie: 5
  // printf("intr before: %lx ", reg_scr(SSTATUS_ID));
  if(reg_scr(SSTATUS_ID) & 2){
    reg_scr(SSTATUS_ID) |= (1 << 5);
  }
  else{
    reg_scr(SSTATUS_ID) &= ~(intptr_t)(1 << 5);
  }
  reg_scr(SSTATUS_ID) &= ~(intptr_t)2;
  
  // printf("intr aft: %lx \n", reg_scr(SSTATUS_ID));
  // printf("stvec %lx\n", reg_scr(STVEC_ID));
  rtl_j(s, reg_scr(STVEC_ID));
}

void query_intr(DecodeExecState *s) {
  // if( cpu.INTR && reg_scr(SSTATUS_ID) != 0) printf("INTR: %d status: %lx %lx\n", cpu.INTR, reg_scr(SSTATUS_ID), cpu.pc);
  if(cpu.INTR && reg_scr(SSTATUS_ID) & 2){
    cpu.INTR = 0;
    raise_intr(s, IRQ_TIMER, cpu.pc);
    update_pc(s);
    
  }
}
