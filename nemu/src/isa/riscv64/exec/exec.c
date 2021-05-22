#include <cpu/exec.h>
#include "../local-include/decode.h"
#include "all-instr.h"

static inline void set_width(DecodeExecState *s, int width) {
  s->width = width;
}

static inline def_EHelper(load) {
  switch (s->isa.instr.i.funct3 & 3) {
    EXW  (0, ld, 1)
    EXW  (1, ld, 2)
    EXW  (2, ld, 4)
    EXW  (3, ld, 8) 
    default: exec_inv(s);
  }
}

static inline def_EHelper(store) {
  switch (s->isa.instr.s.funct3) {
    EXW  (0, st, 1)   //sb, width=1
    EXW  (1, st, 2)   //sh, width=2
    EXW  (2, st, 4)   //sw, width=4
    EXW  (3, st, 8)   //sd, width=8
    default: exec_inv(s);
  }
}


static inline def_EHelper(slt){
  rtl_setrelop(s, RELOP_LT, s->dest.preg, s->src1.preg, s->src2.preg);
  print_asm_template3(slt);
}
static inline def_EHelper(slti){
 rtl_setrelopi(s, RELOP_LT, s->dest.preg, s->src1.preg, s->src2.simm);
 print_asm_template3(slti);
}

static inline def_EHelper(sltu){
  rtl_setrelop(s, RELOP_LTU, s->dest.preg, s->src1.preg, s->src2.preg);
  print_asm_template3(sltu);
}
static inline def_EHelper(sltiu){
  rtl_setrelopi(s, RELOP_LTU, s->dest.preg, s->src1.preg, s->src2.imm);
  print_asm_template3(sltiu);
}


static inline def_EHelper(auipc){
  rtl_addi(s, s->dest.preg, &cpu.pc, s->src1.imm);
  print_asm_template2(auipc);
}

static inline def_EHelper(jal){
  rtl_li(s, s->dest.preg, cpu.pc+4);
  rtl_j(s, cpu.pc + s->src1.imm);
  print_asm_template2(jal);
}

static inline def_EHelper(jalr){
  rtl_li(s, s0, cpu.pc+4); //s0 = pc+4
  rtl_li(s, s1, s->src2.imm);
  rtl_add(s, s1, s->src1.preg, s1);
  rtl_li(s, s2, 1);
  rtl_not(s, s2, s2);
  rtl_and(s, s1, s1, s2);

  rtl_jr(s, s1);
  
  rtl_mv(s, s->dest.preg, s0);
  print_asm_template3(jalr);
}

static int fun2op[8] = {/*000*/ RELOP_EQ, RELOP_NE, 
                        /*010*/ RELOP_FALSE, RELOP_FALSE, 
                        /*100*/ RELOP_LT, RELOP_GE, 
                        /*110*/ RELOP_LTU, RELOP_GEU};
static inline def_EHelper(branch){
  // printf("%d %lx %lx %d\n", s->isa.instr.b.funct3, *(id_src1->preg), *(id_src2->preg), interpret_relop(fun2op[s->isa.instr.b.funct3], id_src1->preg, id_src2->preg));
  rtl_jrelop(s, fun2op[s->isa.instr.b.funct3], id_src1->preg, id_src2->preg, cpu.pc + id_dest->simm);
  switch(s->isa.instr.b.funct3){
    case 0: print_asm_template3(beq); break;
    case 1: print_asm_template3(bne); break;
    case 4: print_asm_template3(blt); break;
    case 5: print_asm_template3(bge); break;
    case 6: print_asm_template3(bltu); break;
    case 7: print_asm_template3(bgeu); break;
    default: assert(0);
  }
}

static inline def_EHelper(csr_inst){
  switch(s->isa.instr.i.funct3){
    case 0:
      if(s->isa.instr.i.simm11_0 == 0) exec_ecall(s);
      else if(s->isa.instr.i.simm11_0 == 0b100000010) exec_sret(s);
      else exec_inv(s);
      break;
    EX (1, csrrw)
    EX (2, csrrs)
    EX (3, csrrc)
    EX (5, csrrwi)
    EX (6, csrrsi)
    EX (7, csrrci)
    default: exec_inv(s);
  }
}

static inline def_EHelper(instw){
  switch(s->isa.instr.r.funct3){
    case 0:
        switch(s->isa.instr.r.funct7){
          EXW (0, add, 4)
          EXW (1, mul, 4)
          EXW (0b100000, sub, 4)
          default: exec_inv(s);
        }
        break;
    case 1:
        switch(s->isa.instr.r.funct7){
          EXW (0, sll, 4)
          default: exec_inv(s);
        }
        break;
    case 4:
        switch(s->isa.instr.r.funct7){
          EXW (1, div, 4)
          default: exec_inv(s);
        }
        break;
    case 5:
        switch(s->isa.instr.r.funct7){
          EXW (0, srl, 4)
          EXW (1, divu, 4)
          EXW (0b100000, sra, 4)
          default: exec_inv(s);
        }
        break;
    case 6:
        switch(s->isa.instr.r.funct7){
          EXW (1, rem, 4)
          default: exec_inv(s);
        }
        break;
    case 7:
        switch(s->isa.instr.r.funct7){
          EXW (1, remu, 4)
          default: exec_inv(s);
        }
        break;
    default: exec_inv(s);
  }
}


static inline def_EHelper(insti){
  // printf("insti %lx\n", cpu.pc);
  switch(s->isa.instr.i.funct3){
    EX (0, addi)
    EX (1, slli)
    EX (2, slti)
    EX (3, sltiu)
    EX (4, xori)
    case 5: 
      switch(s->isa.instr.i.simm11_0 >> 10){
        EX (0, srli)
        EX (1, srai)
        default: exec_inv(s);
      }
      break;
    EX (6, ori)
    EX (7, andi)
    default: exec_inv(s);
  }
}

static inline def_EHelper(instiw){
  switch(s->isa.instr.i.funct3){
    EXW (0, addi, 4)
    EXW (1, slli, 4)
    EXW (2, slti, 4)
    EXW (3, sltiu, 4)
    EXW (4, xori, 4)
    case 5: 
      switch(s->isa.instr.i.simm11_0 >> 10){
        EXW (0, srli, 4)
        EXW (1, srai, 4)
        default: exec_inv(s);
      }
      break;
    default: exec_inv(s);
  }
}


static inline def_EHelper(inst){
  switch(s->isa.instr.r.funct3){
    case 0: 
        switch(s->isa.instr.r.funct7){
          EX (0, add)
          EX (1, mul)
          EX (0b100000, sub)
          default: exec_inv(s);
        }
        break;
    case 1:
        switch(s->isa.instr.r.funct7){
          EX (0, sll)
          // EX (1, mulh)
          default: exec_inv(s);
        }
        break;
    case 2:
        switch(s->isa.instr.r.funct7){
          EX (0, slt)
          // EX (1, mulhsu)
          default: exec_inv(s);
        }
        break;
    case 3:
        switch(s->isa.instr.r.funct7){
          EX (0, sltu)
          // EX (1, mulhu)
          default: exec_inv(s);
        }
        break;
    case 4:
        switch(s->isa.instr.r.funct7){
          EX (0, xor)
          EX (1, div)
          default: exec_inv(s);
        }
        break;
    case 5: 
         switch(s->isa.instr.r.funct7){
          EX (0, srl)
          EX (1, divu)
          EX (0b100000, sra)
          default: exec_inv(s);
        }
        break;
    case 6:
        switch(s->isa.instr.r.funct7){
          EX (0, or)
          EX (1, rem)
          default: exec_inv(s);
        }
        break;
    case 7:
        switch(s->isa.instr.r.funct7){
          EX (0, and)
          EX (1, remu)
          default: exec_inv(s);
        }
        break;
    default: exec_inv(s);

  }
}

static inline void fetch_decode_exec(DecodeExecState *s) {
  s->isa.instr.val = instr_fetch(&s->seq_pc, 4);
  Assert(s->isa.instr.i.opcode1_0 == 0x3, "Invalid instruction");
  // printf("decode: %x\n", s->isa.instr.val);
  switch (s->isa.instr.i.opcode6_2) {
    IDEX  (0b00000, I, load)
    IDEX  (0b00100, I, insti)
    IDEX  (0b00101, U, auipc)
    IDEX  (0b00110, I, instiw)
    IDEX  (0b01000, S, store)
    IDEX  (0b01100, R, inst)
    IDEX  (0b01101, U, lui)
    IDEX  (0b01110, R, instw)
    IDEX  (0b11000, B, branch)
    IDEX  (0b11001, I, jalr)
    EX    (0b11010, nemu_trap)
    IDEX  (0b11011, J, jal)
    IDEX  (0b11100, I, csr_inst);
    default: exec_inv(s);
  }
  // printf("decode end: %x\n", s->isa.instr.val);
}

static inline void reset_zero() {
  reg_d(0) = 0;
}

vaddr_t isa_exec_once() {
  
  DecodeExecState s;
  s.is_jmp = 0;
  s.seq_pc = cpu.pc;

  fetch_decode_exec(&s);
  update_pc(&s);

  reset_zero();

  return s.seq_pc;
}
