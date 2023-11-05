#include <cpu/exec.h>
#include <monitor/monitor.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"

def_EHelper(inv) {
  /* invalid opcode */

  // uint32_t instr[2];
  // s->seq_pc = cpu.pc;
  // instr[0] = instr_fetch(s, &s->seq_pc, 4);
  // instr[1] = instr_fetch(s, &s->seq_pc, 4);

  // printf("invalid opcode(PC = " FMT_WORD ": %08x %08x ...\n\n",
  //     cpu.pc, instr[0], instr[1]);

  // display_inv_msg(cpu.pc);

  // rtl_exit(NEMU_ABORT, cpu.pc, -1);

  // print_asm("invalid opcode");
  s->is_trap = 1;
  s->trap.cause = CAUSE_ILLEGAL_INSTRUCTION;
  s->trap.tval = s->isa.instr.i.opcode1_0 == 0x3 ? s->isa.instr.val : s->c_inst.val;
}

def_EHelper(nemu_trap) {
  difftest_skip_ref();

  rtl_exit(NEMU_END, cpu.pc, reg_d(10)); // grp[10] is $a0

  print_asm("nemu trap");
  return;
}

def_EHelper(nemu_exit){
  rtl_exit(NEMU_END, cpu.pc, 0);
}

def_EHelper(fence){
  return;
}

def_EHelper(nop){
  return;
}

def_EHelper(illegal){
  s->is_trap = 1;
  s->trap.cause = CAUSE_ILLEGAL_INSTRUCTION;
  s->trap.tval = s->isa.instr.i.opcode1_0 == 0x3 ? s->isa.instr.val : s->c_inst.val;
}
