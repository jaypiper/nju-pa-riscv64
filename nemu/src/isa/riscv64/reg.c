#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
  for(int i = 0; i < 32; i++){
    printf("%-4s    %016lx    %lu\n", regs[i], reg_d(i), reg_d(i));
    // if(i % 2) printf("\n");
    // else printf("      ");
  }
  printf("pc      %16lx    %lu\n", cpu.pc, cpu.pc);
  printf("cause   %16lx    \n", reg_scr(SCAUSE_ID));
  printf("status  %16lx    \n", reg_scr(SSTATUS_ID));
  printf("epc     %16lx    \n", reg_scr(SEPC_ID));
}

word_t isa_reg_str2val(const char *s, bool *success) {
  assert(strlen(s) >= 1);
  if(strcmp(s, "$0") == 0) {
    *success = 1;
    return reg_d(0);
  }
  for(int i = 1; i < 32; i++){
    if(strcmp(s+1, regs[i]) == 0){
      *success = 1;
      return reg_d(i);
    }
  }
  if(strcmp(s+1, "pc") == 0){
    *success = 1;
    return cpu.pc;
  }
  assert(0);
  return 0;
}
