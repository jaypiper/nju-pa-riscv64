static inline def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
  print_asm_template2(lui);
}

static inline def_EHelper(add){
  if(s->width == 4) {
    rtl_addw(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(addw);
  }
  else {
    rtl_add(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(add);
  }
}
static inline def_EHelper(addi){
  if(s->width == 4){
    rtl_addiw(s, s->dest.preg, s->src1.preg, s->src2.simm);
    print_asm_template3(addiw);
  }
  else{
    rtl_addi(s, s->dest.preg, s->src1.preg, s->src2.simm);
    print_asm_template3(addi);
  }
}

static inline def_EHelper(sub){
  if(s->width == 4) {
    rtl_subw(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(subw);
  }
  else {
    rtl_sub(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(sub);
  }
}

static inline def_EHelper(mul){
  if(s->width == 4){
    rtl_mulw(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(mulw);
  }
  else{
    rtl_mul_lo(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(mul);
  }
}

static inline def_EHelper(mulh){
  rtl_imul_hi(s, s->dest.preg, s->src1.preg, s->src2.preg);
}

static inline def_EHelper(mulhu){
  rtl_mul_hi(s, s->dest.preg, s->src1.preg, s->src2.preg);
}

static inline def_EHelper(div){
  if(s->width == 4){
    rtl_divw(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(divw);
  }
  else{
    rtl_idiv_q(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(div);
  }
}
static inline def_EHelper(divu){
  if(s->width == 4){
    rtl_divuw(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(divuw);
  }
  else{
    rtl_div_q(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(divu);
  }
}
static inline def_EHelper(rem){
  if(s->width == 4){
    rtl_remw(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(remw);
  }
  else{
    rtl_idiv_r(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(rem);
  }
}
static inline def_EHelper(remu){
  if(s->width == 4){
    rtl_remuw(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(remuw);
  }
  else{
    rtl_div_r(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(remu);
  }
}

static inline def_EHelper(sll){
  if(s->width == 4) {
    rtl_shlw(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(sllw);
  }
  else {
    rtl_shl(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(sll);
  }
}
static inline def_EHelper(slli){
  if(s->width == 4){
    rtl_shliw(s, s->dest.preg, s->src1.preg, s->src2.imm & 0x3f);
    print_asm_template3(slliw);
  }
  else{
    rtl_shli(s, s->dest.preg, s->src1.preg, s->src2.imm);
    print_asm_template3(slli);
  }
}

static inline def_EHelper(xor){
  rtl_xor(s, s->dest.preg, s->src1.preg, s->src2.preg);
  print_asm_template3(xor);
}
static inline def_EHelper(xori){
  rtl_xori(s, s->dest.preg, s->src1.preg, s->src2.imm);
  print_asm_template3(xori);
}

static inline def_EHelper(sra){
  if(s->width == 4){
    rtl_sarw(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(sarw);
  }
  else{
    rtl_sar(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(sar);
  }
}
static inline def_EHelper(srai){
  if(s->width == 4){
    rtl_sariw(s, id_dest->preg, id_src1->preg, s->src2.imm & 0x3f);
    print_asm_template3(sraiw);
  }
  else{
    rtl_sari(s, id_dest->preg, id_src1->preg, s->src2.imm & 0x3f);
    print_asm_template3(srai);
  }
}

static inline def_EHelper(srl){
  if(s->width == 4){
    rtl_shrw(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(srlw);
  }
  else{
    rtl_shr(s, s->dest.preg, s->src1.preg, s->src2.preg);
    print_asm_template3(srl);
  }
}
static inline def_EHelper(srli){
  if(s->width == 4){
    rtl_shriw(s, id_dest->preg, id_src1->preg, s->src2.imm & 0x3f);
    print_asm_template3(srliw);
  }
  else{
    rtl_shri(s, id_dest->preg, id_src1->preg, s->src2.imm & 0x3f);
    print_asm_template3(srli);
  }
  
}

static inline def_EHelper(or){
  rtl_or(s, s->dest.preg, s->src1.preg, s->src2.preg);
  print_asm_template3(or);
}
static inline def_EHelper(ori){
  rtl_ori(s, s->dest.preg, s->src1.preg, s->src2.simm);
  print_asm_template3(ori);
}

static inline def_EHelper(and){
  rtl_and(s, s->dest.preg, s->src1.preg, s->src2.preg);
  print_asm_template3(and);
}
static inline def_EHelper(andi){
  rtl_andi(s, s->dest.preg, s->src1.preg, s->src2.simm);
  print_asm_template3(andi);
}

