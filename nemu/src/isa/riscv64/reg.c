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
  id = id & 0xfff;
  rtlreg_t w_sstatus_mask = SSTATUS_SIE | SSTATUS_SPIE | SSTATUS_SPP | SSTATUS_FS
              | SSTATUS_XS | SSTATUS_SUM | SSTATUS_MXR | SSTATUS_VS;
  rtlreg_t w_mstatus_mask = MSTATUS_MIE | MSTATUS_MPIE | MSTATUS_MPRV | MSTATUS_SIE | MSTATUS_SPIE
                  | MSTATUS_TW | MSTATUS_TSR | MSTATUS_MXR | MSTATUS_SUM | MSTATUS_TVM
                  | MSTATUS_FS | MSTATUS_VS | MSTATUS_SPP | MSTATUS_MPP;
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
      cpu.csr[id] = set_partial_val(cpu.csr[CSR_SSTATUS], w_sstatus_mask, val);
      cpu.csr[CSR_MSTATUS] = set_partial_val(cpu.csr[CSR_MSTATUS], w_sstatus_mask, val);
      bool dirty = (cpu.csr[CSR_MSTATUS] & MSTATUS_FS) == MSTATUS_FS;
      dirty |= (cpu.csr[CSR_MSTATUS] & MSTATUS_XS) == MSTATUS_XS;
      cpu.csr[CSR_MSTATUS] = set_val(cpu.csr[CSR_MSTATUS], MSTATUS64_SD, dirty);
      cpu.csr[CSR_SSTATUS] = set_val(cpu.csr[CSR_SSTATUS], SSTATUS64_SD, dirty);
      break;
    }
    case CSR_MSTATUS: {
      cpu.csr[id] = set_partial_val(cpu.csr[CSR_MSTATUS], w_mstatus_mask, val);

      bool dirty = (cpu.csr[CSR_MSTATUS] & MSTATUS_FS) == MSTATUS_FS;
      dirty |= (cpu.csr[CSR_MSTATUS] & MSTATUS_XS) == MSTATUS_XS;
      cpu.csr[CSR_MSTATUS] = set_val(cpu.csr[CSR_MSTATUS], MSTATUS64_SD, dirty);
      w_sstatus_mask |= SSTATUS64_SD;
      cpu.csr[CSR_SSTATUS] = set_partial_val(cpu.csr[CSR_SSTATUS], w_sstatus_mask, cpu.csr[CSR_MSTATUS]);
      break;
    }
    case CSR_SIP: {
      rtlreg_t mask = supervisor_ints;
      cpu.csr[id] = set_partial_val(cpu.csr[id], mask, val);
      cpu.csr[CSR_MIP] = set_partial_val(cpu.csr[CSR_MIP], mask, val);
      break;
    }
    case CSR_MIP: {
      rtlreg_t mask = supervisor_ints;
      cpu.csr[id] = set_partial_val(cpu.csr[CSR_MIP], mask, val);
      cpu.csr[CSR_SIP] = set_partial_val(cpu.csr[CSR_SIP], mask, val);
      break;
    }
    case CSR_SATP: {
      rtlreg_t mask = SATP64_PPN | SATP64_MODE;
      cpu.csr[id] = set_partial_val(cpu.csr[CSR_SATP], mask, val);
      break;
    }
    case CSR_PMPADDR0:
    case CSR_PMPADDR1:
    case CSR_PMPADDR2:
    case CSR_PMPADDR3:{
      rtlreg_t mask = (uint64_t)(-1) >> 10;
      cpu.csr[id] = set_partial_val(cpu.csr[id], mask, val);
      break;
    }
    case CSR_MCOUNTEREN:
    case CSR_SCOUNTEREN:
      cpu.csr[id] = val & 0xffffffff;
      break;

    default: cpu.csr[id] = val;
  }
}

rtlreg_t get_csr(int id){
  return cpu.csr[id&0xfff];
}

void inline set_priv(int priv){
  cpu.privilege = priv;
}

word_t inline get_priv(){
  return cpu.privilege;
}

#define CSR_ACCESS_MASK 0xc00
#define CSR_ACCESS_SHIFT 10
#define GET_CSR_ACCESS(id) (((id) & CSR_ACCESS_MASK) >> CSR_ACCESS_SHIFT)
#define CSR_PRIV_MASK 0x300
#define CSR_PRIV_SHIFT 8
#define GET_CSR_PRIV(id) (((id) & CSR_PRIV_MASK) >> CSR_PRIV_SHIFT)

static int valid_csr[] = {
  CSR_SEPC, CSR_STVEC, CSR_SCAUSE, CSR_STVAL, CSR_SSCRATCH, CSR_SSTATUS, CSR_SATP,
  CSR_SIE, CSR_SIP, CSR_MTVEC, CSR_MEPC, CSR_MCAUSE, CSR_MIE, CSR_MIP, CSR_MTVAL,
  CSR_MSCRATCH, CSR_MSTATUS, CSR_MHARTID, CSR_MEDELEG, CSR_MIDELEG, CSR_PMPADDR0,
  CSR_PMPADDR1, CSR_PMPADDR2, CSR_PMPADDR3, CSR_PMPCFG0, CSR_USCRATCH, CSR_MISA,
  CSR_SCOUNTEREN, CSR_MCOUNTEREN
};
static int valid_csr_num = sizeof(valid_csr) / sizeof(valid_csr[0]);

static inline int get_valid_csr(int id){
  for(int i = 0; i < valid_csr_num; i++){
    if(valid_csr[i] == id) return i;
  }
  return -1;
}

void set_csr_cond(DecodeExecState* s, int id, rtlreg_t val){
  id = id & 0xfff;
  if(GET_CSR_PRIV(id) > get_priv() || GET_CSR_ACCESS(id) == 3 || get_valid_csr(id) == -1){
    s->is_trap = true;
    s->trap.cause = CAUSE_ILLEGAL_INSTRUCTION;
    s->trap.tval = s->isa.instr.val;
    return;
  }
  set_csr(id, val);
}

rtlreg_t get_csr_cond(DecodeExecState* s, int id){
  id = id & 0xfff;
  if(GET_CSR_PRIV(id) > get_priv() || get_valid_csr(id) == -1){
    s->is_trap = true;
    s->trap.cause = CAUSE_ILLEGAL_INSTRUCTION;
    s->trap.tval = s->isa.instr.val;
    return 0;
  }
  return get_csr(id);
}

rtlreg_t inline get_pre_lr(){
  return cpu.pre_lr;
}
bool inline pre_lr_valid(){
  return cpu.pre_lr_valid;
}
void inline set_pre_lr(rtlreg_t pre_lr){
  cpu.pre_lr = pre_lr;
  cpu.pre_lr_valid = 1;
}
void inline clear_pre_lr(){
  cpu.pre_lr_valid = 0;
}

void nemu_raise_intr(int NO){
  cpu.csr[CSR_MIP] = get_csr(CSR_MIP) | (1 << NO);
}

void nemu_clear_intr(int NO){
  cpu.csr[CSR_MIP] = get_csr(CSR_MIP) & ~(uint64_t)(1 << NO);
}

void inline set_lr_paddr(paddr_t paddr){
  cpu.pre_lr = paddr;
  cpu.pre_lr_valid = true;
}

void set_lr_vaddr(DecodeExecState* s, vaddr_t vaddr, int len){
  paddr_t paddr = vaddr_translate(s, vaddr, MEM_TYPE_READ, len);
  if(s->is_trap) return;
  set_lr_paddr(paddr);
}

bool check_lr_vaddr(DecodeExecState* s, vaddr_t vaddr, int len){
  paddr_t paddr = vaddr_translate(s, vaddr, MEM_TYPE_WRITE, len);
  return cpu.pre_lr_valid && (paddr == cpu.pre_lr);
}