#include <isa.h>
#include "local-include/reg.h"
#include <encoding.h>
#include <csr.h>

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

const int csrs[] = { CSR_SSTATUS, CSR_SIE, CSR_STVEC, CSR_SSCRATCH, CSR_SEPC, 
              CSR_SCAUSE, CSR_STVAL, CSR_SIP, CSR_SATP,
              CSR_MEDELEG, CSR_MIDELEG, CSR_MIE, CSR_MTVEC, CSR_MSCRATCH, CSR_MEPC,
              CSR_MCAUSE, CSR_MTVAL, CSR_MIP, /*CSR_PMPCFG0, CSR_PMPADDR0, */CSR_MSTATUS,
              CSR_MHARTID
              };

int csr_num = sizeof(csrs)/sizeof(int);
const char* csr_name[] = {
            "sstatus", "sie", "stvec", "sscratch", "sepc",
            "scause", "stval", "sip", "stap",
            "medeleg", "mideleg", "mie", "mtvec", "mscratch", "mepc",
            "mcause", "mtval", "mip",/* "pmpcfg0", "pmpaddr0", */"mstatus", "mhartid"};


void isa_reg_display(CPU_state* ref_r) {
  if(ref_r){
    for(int i = 0; i < 32; i++){
      printf("%-4s    %016lx   |  %016lx\n", regs[i], ref_r->gpr[i], reg_d(i));
      // if(i % 2) printf("\n");
      // else printf("      ");
    }
    printf("pc      %16lx   |  %16lx\n", ref_r->pc, cpu.pc);
    for(int i = 0; i < csr_num; i++){
      int id = csrs[i];
      printf("%-8s    %016lx   |  %016lx\n", csr_name[i], ref_r->csr[id], get_csr(id));
    }
    printf("prv        %lx      |  %lx\n", ref_r->privilege, cpu.privilege);

  }
  else{
    for(int i = 0; i < 16; i++){
      printf("%-4s    %016lx   | %-4s   %016lx\n", regs[i], reg_d(i), regs[i+16], reg_d(i+16));
      // if(i % 2) printf("\n");
      // else printf("      ");
    }
    for(int i = 0; i < csr_num/2; i++){
      int id1 = csrs[i];
      int id2 = csrs[i+csr_num/2];
      printf("%-8s  %016lx   | %-8s %016lx\n", csr_name[i], get_csr(id1), csr_name[i+csr_num/2], get_csr(id2));
    }
    printf("pc      %16lx    | prv   %lx\n", cpu.pc, cpu.privilege);
  }
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

void set_csr(int id, rtlreg_t val){
  rtlreg_t supervisor_ints = MIP_SSIP | MIP_STIP | MIP_SEIP;
  switch(id){
    case CSR_MEDELEG: {
        rtlreg_t mask = mask =
          (1 << CAUSE_MISALIGNED_FETCH) |
          (1 << CAUSE_BREAKPOINT) |
          (1 << CAUSE_USER_ECALL) |
          (1 << CAUSE_SUPERVISOR_ECALL) |
          (1 << CAUSE_FETCH_PAGE_FAULT) |
          (1 << CAUSE_LOAD_PAGE_FAULT) |
          (1 << CAUSE_STORE_PAGE_FAULT);
        cpu.csr[id] = set_partial_val(cpu.csr[id], mask, val);
        break;
    }
    case CSR_MIDELEG: {
      rtlreg_t mask = supervisor_ints;
      cpu.csr[id] = set_partial_val(cpu.csr[id], mask, val);
      break;
    }
    case CSR_SIE: {
      rtlreg_t mask = cpu.csr[CSR_MIDELEG];
      cpu.csr[id] = set_partial_val(cpu.csr[id], mask, val);
      cpu.csr[CSR_MIE] = set_partial_val(cpu.csr[CSR_MIE], mask, cpu.csr[CSR_SIE]);
      break;
    }
    case CSR_SSTATUS: {
      rtlreg_t mask = SSTATUS_SIE | SSTATUS_SPIE | SSTATUS_SPP | SSTATUS_FS
                 | SSTATUS_XS | SSTATUS_SUM | SSTATUS_MXR | SSTATUS_VS;
      cpu.csr[id] = set_partial_val(cpu.csr[CSR_SSTATUS], mask, val);
      cpu.csr[CSR_MSTATUS] = set_partial_val(cpu.csr[CSR_MSTATUS], mask, val);
      break;
    }
    case CSR_SIP: {
      rtlreg_t mask = supervisor_ints;
      cpu.csr[id] = set_partial_val(cpu.csr[id], mask, val);
      cpu.csr[CSR_MIP] = set_partial_val(cpu.csr[CSR_MIP], mask, val);
    }

    default: cpu.csr[id] = val;
  }
}

rtlreg_t get_csr(int id){
  return cpu.csr[id];
}

void set_priv(int priv){
  cpu.privilege = priv;
}

word_t get_priv(){
  return cpu.privilege;
}
