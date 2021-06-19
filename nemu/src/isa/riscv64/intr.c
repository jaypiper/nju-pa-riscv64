#include <cpu/exec.h>
#include "local-include/rtl.h"
#define IRQ_TIMER 0x8000000000000005

void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  reg_scr(SEPC_ID) = epc;
  reg_scr(SCAUSE_ID) = NO;

  rtl_j(s, reg_scr(STVEC_ID));
}

void query_intr(DecodeExecState *s) {
  // if(cpu.INTR){
  //   cpu.INTR = 0;
  //   raise_intr(s, IRQ_TIMER, cpu.pc);
  //   update_pc(s);
  //   //sie: 1 spie: 5
  //   if(reg_scr(SSTATUS_ID) & 2){
  //     reg_scr(SSTATUS_ID) |= (1 << 5);
  //   }
  //   else{
  //     reg_scr(SSTATUS_ID) &= ~(intptr_t)(1 << 5);
  //   }
  // }
}
