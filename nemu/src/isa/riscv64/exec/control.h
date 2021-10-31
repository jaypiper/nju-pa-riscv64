void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc);

static inline def_EHelper(ecall){
  // bool success;

  s->is_trap = 1;

  // rtlreg_t cause, medeleg, stvec, sstatus;
  switch(cpu.privilege){
    case PRV_U: s->trap.cause = CAUSE_USER_ECALL; break;
    case PRV_S: s->trap.cause = CAUSE_SUPERVISOR_ECALL; break;
    case PRV_M: s->trap.cause = CAUSE_MACHINE_ECALL; break;
    default: assert(0);
  }
  s->trap.tval = 0;
  print_asm_template1(ecall);
}

static inline def_EHelper(sret){
  rtl_j(s, get_csr(SEPC_ID));
  rtlreg_t sstatus = get_csr(CSR_SSTATUS);
  rtlreg_t prev_prv = get_val(sstatus, SSTATUS_SPP);
  sstatus = set_val(sstatus, SSTATUS_SIE, get_val(sstatus, SSTATUS_SPIE));
  sstatus = set_val(sstatus, SSTATUS_SPIE, 1);
  sstatus = set_val(sstatus, SSTATUS_SPP, PRV_U);
  set_csr(CSR_SSTATUS, sstatus);
  set_priv(prev_prv);
  print_asm_template1(sret);
}

static inline def_EHelper(mret){
  rtl_j(s, get_csr(MEPC_ID));
  rtlreg_t mstatus = get_csr(CSR_MSTATUS);
  rtlreg_t prev_prv = get_val(mstatus, MSTATUS_MPP);
  mstatus = set_val(mstatus, MSTATUS_MIE, get_val(mstatus, MSTATUS_MPIE));
  mstatus = set_val(mstatus, MSTATUS_MPIE, 1);
  mstatus = set_val(mstatus, MSTATUS_MPP, PRV_U);
  set_csr(CSR_MSTATUS, mstatus);
  set_priv(prev_prv);
  print_asm_template1(mret);
}

static inline def_EHelper(wfi){
  return;
}

static inline def_EHelper(csrrc){
  rtl_li(s, s0, get_csr_cond(s, s->src2.imm)); //t
  if(s->is_trap) return;
  rtl_not(s, s1, s->src1.preg);
  rtl_and(s, s1, s0, s1);
  set_csr_cond(s, s->src2.imm, *s1);
  if(s->is_trap) return;
  rtl_mv(s, s->dest.preg, s0);
  print_asm_template3(csrrc);
}

static inline def_EHelper(csrrci){
  rtl_li(s, s0, get_csr_cond(s, s->src2.imm));
  if(s->is_trap) return;
  rtl_andi(s, s1, s0, ~(s->src1.reg & 0x1f));
  set_csr_cond(s, s->src2.imm, *s1);
  rtl_mv(s, s->dest.preg, s0);
  print_asm_template3(csrrci);
}

static inline def_EHelper(csrrs){
  rtl_li(s, s0, get_csr_cond(s, s->src2.imm)); //t
  if(s->is_trap) return;
  rtl_or(s, s1, s0, s->src1.preg);
  if(s->src1.reg != 0)
    set_csr_cond(s, s->src2.imm, *s1);
  if(s->is_trap) return;
  rtl_mv(s, s->dest.preg, s0);
  print_asm_template3(csrrs);
}

static inline def_EHelper(csrrsi){
  rtl_li(s, s0, get_csr_cond(s, s->src2.imm)); //t
  if(s->is_trap) return;
  rtl_ori(s, s1, s0, s->src1.reg & 0x1f);
  set_csr_cond(s, s->src2.imm, *s1);
  if(s->is_trap) return;
  rtl_mv(s, s->dest.preg, s0);
  print_asm_template3(csrrsi);
}

static inline def_EHelper(csrrw){
  rtl_li(s, s0, get_csr_cond(s, s->src2.imm)); //t
  if(s->is_trap) return;
  rtl_mv(s, s1, s->src1.preg);
  set_csr_cond(s, s->src2.imm, *s1);
  if(s->is_trap) return;
  rtl_mv(s, s->dest.preg, s0);
  print_asm_template3(csrrw);
}

static inline def_EHelper(csrrwi){
  rtl_li(s, s->dest.preg, get_csr_cond(s, s->src2.imm));
  if(s->is_trap) return;
  set_csr_cond(s, s->src2.imm, s->src1.reg);
  if(s->is_trap) return;
  print_asm_template3(csrrwi);
}

static inline def_EHelper(sfence_vma){
  rtlreg_t mstatus = get_csr(CSR_MSTATUS);
  word_t required_priv = get_val(mstatus, MSTATUS_TVM) ? PRV_M : PRV_S;
  word_t cur_priv = get_priv();
  if(cur_priv > required_priv) return;

  // rtlreg_t pc = cpu.pc;
}