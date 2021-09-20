void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc);

static inline def_EHelper(ecall){
  // bool success;

  rtlreg_t cause, medeleg, stvec, sstatus;
  switch(cpu.privilege){
    case PRV_U: cause = CAUSE_USER_ECALL; break;
    case PRV_S: cause = CAUSE_SUPERVISOR_ECALL; break;
    default: assert(0);
  }
  medeleg = get_csr(CSR_MEDELEG);
  stvec = get_csr(CSR_STVEC);
  sstatus = get_csr(CSR_SSTATUS);
  if(cpu.privilege <= PRV_S && (medeleg >> cause)&1){
    // handle trap in S-mode
    rtlreg_t seq_pc = stvec + (stvec & 1? 4*cause: 0);
    rtl_j(s, seq_pc);
    set_csr(CSR_SCAUSE, cause);
    set_csr(CSR_SEPC, cpu.pc);
    sstatus = set_val(sstatus, SSTATUS_SPIE, get_val(sstatus, SSTATUS_SIE));
    sstatus = set_val(sstatus, SSTATUS_SPP, cpu.privilege);
    sstatus = set_val(sstatus, SSTATUS_SIE, 0);
    set_csr(CSR_SSTATUS, sstatus);
    set_csr(CSR_STVAL, 0);
    set_priv(PRV_S);
  }else{
    assert(0);
  }
  print_asm_template1(ecall);
}

static inline def_EHelper(sret){
  // printf("sret before: %lx ", get_csr(CSR_SSTATUS));
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
  // printf("mret: %lx %lx\n", cpu.pc, get_csr(MEPC_ID));
  rtlreg_t mstatus = get_csr(CSR_MSTATUS);
  rtlreg_t prev_prv = get_val(mstatus, MSTATUS_MPP);
  mstatus = set_val(mstatus, MSTATUS_MIE, get_val(mstatus, MSTATUS_MPIE));
  mstatus = set_val(mstatus, MSTATUS_MPIE, 1);
  mstatus = set_val(mstatus, MSTATUS_MPP, PRV_U);
  set_csr(CSR_MSTATUS, mstatus);
  set_priv(prev_prv);

  print_asm_template1(mret);
}

static inline def_EHelper(csrrc){
  rtl_li(s, s0, get_csr(s->src2.imm)); //t
  rtl_not(s, s1, s->src1.preg);
  rtl_and(s, s1, s0, s1);
  set_csr(s->src2.imm, *s1);
  rtl_mv(s, s->dest.preg, s0);
  print_asm_template3(csrrc);
}

static inline def_EHelper(csrrci){
  rtl_li(s, s0, get_csr(s->src2.imm));
  rtl_andi(s, s1, s0, ~(s->src1.reg));
  set_csr(s->src2.imm, (get_csr(s->src2.imm) &(~0x1f)) | (*s1 & 0x1f));
  rtl_mv(s, s->dest.preg, s0);
  print_asm_template3(csrrci);
}

static inline def_EHelper(csrrs){
  rtl_li(s, s0, get_csr(s->src2.imm)); //t
  rtl_or(s, s1, s0, s->src1.preg);
  set_csr(s->src2.imm, *s1);
  rtl_mv(s, s->dest.preg, s0);
  print_asm_template3(csrrs);
}

static inline def_EHelper(csrrsi){
  rtl_li(s, s0, get_csr(s->src2.imm)); //t
  rtl_ori(s, s1, s0, s->src1.reg);
  set_csr(s->src2.imm, (get_csr(s->src2.imm) &(~0x1f)) | (*s1 & 0x1f));
  rtl_mv(s, s->dest.preg, s0);
  print_asm_template3(csrrsi);
}

static inline def_EHelper(csrrw){
  rtl_li(s, s0, get_csr(s->src2.imm)); //t
  rtl_mv(s, s1, s->src1.preg);
  set_csr(s->src2.imm, *s1);
  rtl_mv(s, s->dest.preg, s0);
  print_asm_template3(csrrw);
}

static inline def_EHelper(csrrwi){
  rtl_li(s, s->dest.preg, get_csr(s->src2.imm));
  set_csr(s->src2.imm, s->src1.reg);
  print_asm_template3(csrrwi);
}

static inline def_EHelper(sfence_vma){
  rtlreg_t mstatus = get_csr(CSR_MSTATUS);
  word_t required_priv = get_val(mstatus, MSTATUS_TVM) ? PRV_M : PRV_S;
  word_t cur_priv = get_priv();
  if(cur_priv > required_priv) return;

  // rtlreg_t pc = cpu.pc;
}