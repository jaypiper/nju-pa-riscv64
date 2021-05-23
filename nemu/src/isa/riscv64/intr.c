#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  reg_scr(SEPC_ID) = epc;
  reg_scr(SCAUSE_ID) = NO;

  rtl_j(s, reg_scr(STVEC_ID));
}

void query_intr(DecodeExecState *s) {
}
