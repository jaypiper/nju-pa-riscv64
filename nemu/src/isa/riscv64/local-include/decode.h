#include <cpu/exec.h>
#include "rtl.h"

// decode operand helper
#define def_DopHelper(name) \
  void concat(decode_op_, name) (DecodeExecState *s, Operand *op, word_t val, bool load_val)

static inline def_DopHelper(i) {
  op->type = OP_TYPE_IMM;
  op->imm = val;

  print_Dop(op->str, OP_STR_SIZE, "%ld", op->imm);
}

static inline def_DopHelper(r) {
  op->type = OP_TYPE_REG;
  op->reg = val;
  op->preg = &reg_d(val);

  print_Dop(op->str, OP_STR_SIZE, "%s", reg_name(op->reg));
}

static inline def_DHelper(I) {
  decode_op_r(s, id_src1, s->isa.instr.i.rs1, true);
  decode_op_i(s, id_src2, (sword_t)s->isa.instr.i.simm11_0, true);
  decode_op_r(s, id_dest, s->isa.instr.i.rd, false);
}


static inline def_DHelper(R){
  decode_op_r(s, id_src1, s->isa.instr.r.rs1, true);
  decode_op_r(s, id_src2, s->isa.instr.r.rs2, true);
  decode_op_r(s, id_dest, s->isa.instr.r.rd, true);
}

static inline def_DHelper(U) {
  decode_op_i(s, id_src1, (sword_t)s->isa.instr.u.simm31_12 << 12, true);
  decode_op_r(s, id_dest, s->isa.instr.u.rd, false);

  print_Dop(id_src1->str, OP_STR_SIZE, "0x%x", s->isa.instr.u.simm31_12);
}

static inline def_DHelper(B){
  decode_op_r(s, id_src1, s->isa.instr.b.rs1, true);
  decode_op_r(s, id_src2, s->isa.instr.b.rs2, true);
  decode_op_i(s, id_dest, s->isa.instr.b.simm12 << 12 | s->isa.instr.b.imm11 << 11 | s->isa.instr.b.imm10_5 << 5 | s->isa.instr.b.imm4_1 << 1, true); 
}

static inline def_DHelper(S) {
  decode_op_r(s, id_src1, s->isa.instr.s.rs1, true);
  sword_t simm = (s->isa.instr.s.simm11_5 << 5) | s->isa.instr.s.imm4_0;
  decode_op_i(s, id_src2, simm, true);
  decode_op_r(s, id_dest, s->isa.instr.s.rs2, true);
}

static inline def_DHelper(J){
  decode_op_r(s, id_dest, s->isa.instr.j.rd, true);
  *s0 = s->isa.instr.j.simm20;
  *s0 = (*s0 << 20) | (s->isa.instr.j.simm19_12 << 12) | (s->isa.instr.j.simm11 << 11) | (s->isa.instr.j.simm10_1 << 1);
  decode_op_i(s, id_src1, *s0, true);
}

static int r3_table[] = {8, 9, 10, 11, 12, 13, 14, 15};

static inline def_DopHelper(r3) {
  op->type = OP_TYPE_REG;
  op->reg = r3_table[val];
  op->preg = &reg_d(op->reg);
}

static inline def_DHelper(CIW){
  decode_op_r3(s, id_dest, s->c_inst.ciw.rd_3, true);
  decode_op_r(s, id_src1, 2, true);
  union{
    struct{
      uint16_t imm3    : 1;
      uint16_t imm2    : 1;
      uint16_t imm9_6  : 4;
      uint16_t imm5_4  : 2;
    };
    uint16_t val : 8;
  }ciw_imm;
  ciw_imm.val = s->c_inst.ciw.imm8;
  decode_op_i(s, id_src2, ciw_imm.imm9_6 << 6 | ciw_imm.imm5_4 << 4 | ciw_imm.imm3 << 3 | ciw_imm.imm2 << 2, true);
}

static inline def_DHelper(CLS_W){ //load & store 4B
  decode_op_r3(s, id_dest, s->c_inst.cls.rs2_3, true);
  decode_op_r3(s, id_src1, s->c_inst.cls.rs1_3, true);
  union{
    struct{
      uint16_t offset6 : 1;
      uint16_t offset2 : 1;
    };
    uint16_t val : 2;
  }cls_imm2;
  cls_imm2.val = s->c_inst.cls.imm2;
  decode_op_i(s, id_src2, cls_imm2.offset6 << 6 | s->c_inst.cls.imm3 << 3 | cls_imm2.offset2 << 2, true);
}

static inline def_DHelper(CLS_D){
  decode_op_r3(s, id_dest, s->c_inst.cls.rs2_3, true);
  decode_op_r3(s, id_src1, s->c_inst.cls.rs1_3, true);
  decode_op_i(s, id_src2, s->c_inst.cls.imm2 << 6 | s->c_inst.cls.imm3 << 3, true);
}

static inline def_DHelper(CI){
  decode_op_r(s, id_dest, s->c_inst.ci.rs1, true);
  decode_op_i(s, id_src2, (sword_t)(s->c_inst.ci.imm1 << 5 | s->c_inst.ci.imm5), true);
  decode_op_r(s, id_src1, s->c_inst.ci.rs1, true);
}

static inline def_DHelper(CI_U){
  decode_op_r(s, id_dest, s->c_inst.ci.rs1, true);
  decode_op_i(s, id_src2, ((word_t)s->c_inst.ci.imm1 & 1) << 5 | s->c_inst.ci.imm5, true);
  decode_op_r(s, id_src1, s->c_inst.ci.rs1, true);
}

static inline def_DHelper(CI_LW){
  decode_op_r(s, id_dest, s->c_inst.ci.rs1, true);
  decode_op_r(s, id_src1, 2, true);
  union{
    struct{
      uint16_t imm7_6  : 2;
      uint16_t imm4_2  : 3;
    };
    uint16_t val: 5;
  }ci;
  ci.val = s->c_inst.ci.imm5;
  decode_op_i(s, id_src2, ci.imm7_6 << 6 | (s->c_inst.ci.imm1 & 1) << 5 | ci.imm4_2 << 2, true);
}

static inline def_DHelper(CI_LD){
  decode_op_r(s, id_dest, s->c_inst.ci.rs1, true);
  decode_op_r(s, id_src1, 2, true);
  union{
    struct{
      uint16_t imm8_6  : 3;
      uint16_t imm4_3  : 2;
    };
    uint16_t val: 5;
  }ci;
  ci.val = s->c_inst.ci.imm5;
  decode_op_i(s, id_src2, (word_t)(ci.imm8_6 << 6 | (s->c_inst.ci.imm1 & 1) << 5 | ci.imm4_3 << 3), true);
}

static inline def_DHelper(CSS_SW){
  decode_op_r(s, id_dest, s->c_inst.css.rs2, true);
  decode_op_r(s, id_src1, 2, true);
  union{
    struct{
      uint16_t imm7_6  : 2;
      uint16_t imm5_2  : 4;
    };
    uint16_t val: 6;
  }css;
  css.val = s->c_inst.css.imm6;
  decode_op_i(s, id_src2, css.imm7_6 << 6 | css.imm5_2 << 2, true);
}

static inline def_DHelper(CSS_SD){
  decode_op_r(s, id_dest, s->c_inst.css.rs2, true);
  decode_op_r(s, id_src1, 2, true);
  union{
    struct{
      uint16_t imm8_6  : 3;
      uint16_t imm5_3  : 3;
    };
    uint16_t val: 6;
  }css;
  css.val = s->c_inst.css.imm6;
  decode_op_i(s, id_src2, css.imm8_6 << 6 | css.imm5_3 << 3, true);
}

static inline def_DHelper(CI_ADDSP){
  decode_op_r(s, id_dest, 2, true);
  decode_op_r(s, id_src1, 2, true);
  union{
    struct{
      uint16_t imm5   : 1;
      uint16_t imm8_7 : 2;
      uint16_t imm6   : 1;
      uint16_t imm4   : 1;
    };
    uint16_t val: 5;
  }ci_imm;
  ci_imm.val = s->c_inst.ci.imm5;
  decode_op_i(s, id_src2, (sword_t)(s->c_inst.ci.imm1 << 9 | ci_imm.imm8_7 << 7 | ci_imm.imm6 << 6 | ci_imm.imm5 << 5 | ci_imm.imm4 << 4), true);
}

static inline def_DHelper(CI_LUI){ // store imm in src1 to adapt preivious decoder
  decode_op_r(s, id_dest, s->c_inst.ci.rs1, true);
  decode_op_i(s, id_src1, s->c_inst.ci.imm1 << 17 | s->c_inst.ci.imm5 << 12, true);
}

static inline def_DHelper(CJ){ // imm in src1
  decode_op_r(s, id_dest, 0, true);
  union{
    struct{
      uint16_t imm5   : 1;
      uint16_t imm3_1 : 3;
      uint16_t imm7   : 1;
      uint16_t imm6   : 1;
      uint16_t imm10  : 1;
      uint16_t imm9_8 : 2;
      uint16_t imm4   : 1;
      int16_t imm11   : 1;
    };
    uint16_t val: 11;
  }cj_imm;
  cj_imm.val = s->c_inst.cj.imm11;
  decode_op_i(s, id_src1, (sword_t)(cj_imm.imm11 << 11 | cj_imm.imm10 << 10 | cj_imm.imm9_8 << 8 |
                          cj_imm.imm7 << 7 | cj_imm.imm6 << 6 | cj_imm.imm5 << 5 |
                          cj_imm.imm4 << 4 | cj_imm.imm3_1 << 1), true);
}

static inline def_DHelper(CB){
  decode_op_r3(s, id_src1, s->c_inst.cb.rs1_3, true);
  decode_op_r(s, id_src2, 0, true);
  union{
    struct{
      uint16_t imm4_3 : 2;
      int16_t imm8    : 1;
    };
    uint16_t val: 3;
  }cb1;
  union{
    struct{
      uint16_t imm5   : 1;
      uint16_t imm2_1 : 2;
      uint16_t imm7_6 : 2;
    };
    uint16_t val: 5;
  }cb2;
  cb1.val = s->c_inst.cb.imm3;
  cb2.val = s->c_inst.cb.imm5;
  decode_op_i(s, id_dest, (sword_t)(cb1.imm8 << 8 | cb2.imm7_6 << 6 | cb2.imm5 << 5 |
                            cb1.imm4_3 << 3 | cb2.imm2_1 << 1), true);
}

static inline def_DHelper(CB_SHIFT){
  decode_op_r3(s, id_dest, s->c_inst.cb.rs1_3, true);
  decode_op_r3(s, id_src1, s->c_inst.cb.rs1_3, true);
  decode_op_i(s, id_src2, (s->c_inst.cb.imm3 >> 2) << 5 | s->c_inst.cb.imm5, true);
}

static inline def_DHelper(CB_ANDI){
  decode_op_r3(s, id_dest, s->c_inst.cb.rs1_3, true);
  decode_op_r3(s, id_src1, s->c_inst.cb.rs1_3, true);
  union{
    int16_t simm3 : 3;
  }cb;
  cb.simm3 = s->c_inst.cb.imm3;
  decode_op_i(s, id_src2, (cb.simm3 >> 2) << 5 | s->c_inst.cb.imm5, true);
}

static inline def_DHelper(CLS_R){
  decode_op_r3(s, id_dest, s->c_inst.cls.rs1_3, true);
  decode_op_r3(s, id_src1, s->c_inst.cls.rs1_3, true);
  decode_op_r3(s, id_src2, s->c_inst.cls.rs2_3, true);
}

static inline def_DHelper(CR_JR){
  decode_op_r(s, id_dest, 0, true);
  decode_op_r(s, id_src1, s->c_inst.cr.rs1, true);
  decode_op_i(s, id_src2, 0, true);
}

static inline def_DHelper(CR_JALR){
  decode_op_r(s, id_dest, 1, true);
  decode_op_r(s, id_src1, s->c_inst.cr.rs1, true);
  decode_op_i(s, id_src2, 0, true);
}

static inline def_DHelper(CR_MV){
  decode_op_r(s, id_dest, s->c_inst.cr.rs1, true);
  decode_op_r(s, id_src1, s->c_inst.cr.rs2, true);
  decode_op_i(s, id_src2, 0, true);
}

static inline def_DHelper(CR){
  decode_op_r(s, id_dest, s->c_inst.cr.rs1, true);
  decode_op_r(s, id_src1, s->c_inst.cr.rs1, true);
  decode_op_r(s, id_src2, s->c_inst.cr.rs2, true);
}
