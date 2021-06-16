void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc);

static inline def_EHelper(ecall){
  bool success;
  raise_intr(s, isa_reg_str2val("$a7", &success), s->seq_pc);
  print_asm_template1(ecall);
}

static inline def_EHelper(sret){
  rtl_j(s, reg_scr(SEPC_ID));
  print_asm_template1(sret);
}

static inline def_EHelper(csrrc){
  rtl_li(s, s0, reg_scr(s->src2.imm)); //t
  rtl_not(s, s1, s->src1.preg);
  rtl_and(s, s1, s0, s1);
  reg_scr(s->src2.imm) = *s1;
  rtl_mv(s, s->dest.preg, s0);
  print_asm_template3(csrrc);
}

static inline def_EHelper(csrrci){
  rtl_li(s, s0, reg_scr(s->src2.imm));
  rtl_andi(s, s1, s0, ~(s->src1.reg));
  reg_scr(s->src2.imm) = (reg_scr(s->src2.imm) &(~0x1f)) | (*s1 & 0x1f);
  rtl_mv(s, s->dest.preg, s0);
  print_asm_template3(csrrci);
}

static inline def_EHelper(csrrs){
  rtl_li(s, s0, reg_scr(s->src2.imm)); //t
  rtl_or(s, s1, s0, s->src1.preg);
  reg_scr(s->src2.imm) = *s1;
  rtl_mv(s, s->dest.preg, s0);
  print_asm_template3(csrrs);
}

static inline def_EHelper(csrrsi){
  rtl_li(s, s0, reg_scr(s->src2.imm)); //t
  rtl_ori(s, s1, s0, s->src1.reg);
  reg_scr(s->src2.imm) = (reg_scr(s->src2.imm) &(~0x1f)) | (*s1 & 0x1f);
  rtl_mv(s, s->dest.preg, s0);
  print_asm_template3(csrrsi);
}

static inline def_EHelper(csrrw){
  rtl_li(s, s0, reg_scr(s->src2.imm)); //t
  rtl_mv(s, s1, s->src1.preg);
  reg_scr(s->src2.imm) = *s1;
  rtl_mv(s, s->dest.preg, s0);
  print_asm_template3(csrrw);
}

static inline def_EHelper(csrrwi){
  rtl_li(s, s->dest.preg, reg_scr(s->src2.imm)); 
  reg_scr(s->src2.imm) = s->src1.reg;
  print_asm_template3(csrrwi);
}