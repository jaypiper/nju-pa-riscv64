#include <cpu/exec.h>
#include "../local-include/decode.h"
#include "all-instr.h"
#include <encoding.h>
#include <csr.h>
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
  // assert(0);
  switch(s->isa.instr.i.funct3){
    case 0:
       switch(s->isa.instr.i.simm11_0){
        case 0: exec_ecall(s); break;
        case 0b000100000010: exec_sret(s); break;
        case 0b001100000010: exec_mret(s); break;
        default: if(s->isa.instr.r.funct7 == 0b0001001){ exec_sfence_vma(s);
                }else{ exec_inv(s);}
      }
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

static inline def_EHelper(atomic){
  int amo_width = s->isa.instr.r.funct3 == 0b010? 4 : 8;
  switch(s->isa.instr.r.funct7 >> 2){
    EXW(0b00001, amoswap, amo_width)
    EXW(0b00000, amoadd,  amo_width)
    EXW(0b00100, amoxor,  amo_width)
    EXW(0b01100, amoand,  amo_width)
    EXW(0b01000, amoor,   amo_width)
    EXW(0b10000, amomin,  amo_width)
    EXW(0b10100, amomax,  amo_width)
    EXW(0b11000, amominu, amo_width)
    EXW(0b11100, amomaxu, amo_width)
    default: exec_inv(s);
  }
}

static inline void fetch_decode_exec(DecodeExecState *s) {
  s->isa.instr.val = instr_fetch(s, &s->seq_pc, 4);
  if(s->is_trap){
    s->trap.pc = cpu.pc;
    s->trap.tval = cpu.pc;
    return;
  }
  if(s->isa.instr.i.opcode1_0 != 0x3){
    printf("%x\n", s->isa.instr.val);
  }
  Assert(s->isa.instr.i.opcode1_0 == 0x3, "Invalid instruction");
  //  printf("pc: %lx decode: %x\n", s->seq_pc, s->isa.instr.val);
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
    IDEX  (0b11100, I, csr_inst)
    EX    (0b00011, fence)
    IDEX  (0b01011, R, atomic)
    default: exec_inv(s);
  }
  if(s->is_trap){
    s->trap.pc = cpu.pc;
  }
  // printf("decode end: %x\n", s->isa.instr.val);
}

static inline void reset_zero() {
  reg_d(0) = 0;
}

void take_trap(DecodeExecState* s){
  rtlreg_t cause, deleg, tvec, status;
  cause = s->trap.cause;
  bool is_interrupt = (cause >> 63) != 0;
  deleg = is_interrupt? get_csr(CSR_MIDELEG) : get_csr(CSR_MEDELEG);
  if(!is_interrupt)
    printf("nemu exception cause: %lx pc: %lx\n", s->trap.cause, s->trap.pc);

  if(!(!is_interrupt || (get_priv() == PRV_S && (get_val(get_csr(CSR_SSTATUS), SSTATUS_SIE) || cause == 0x8000000000000007ULL)) || (get_priv() == PRV_M && get_val(get_csr(CSR_MSTATUS), MSTATUS_MIE)) || get_priv() == PRV_U)){
    printf("priv: %ld sstatus: %lx mstatus: %lx cause: %lx\n", get_priv(), get_csr(CSR_SSTATUS), get_csr(CSR_MSTATUS), cause);
    assert(0);
  }
  if(cpu.privilege <= PRV_S && ((deleg >> cause) & 1)){
    tvec = get_csr(CSR_STVEC);
    status = get_csr(CSR_SSTATUS);
    rtlreg_t seq_pc = tvec + (tvec & 1? 4*cause : 0);
    rtl_j(s, seq_pc);
    set_csr(CSR_SCAUSE, cause);
    set_csr(CSR_SEPC, s->trap.pc);
    status = set_val(status, SSTATUS_SPIE, get_val(status, SSTATUS_SIE));
    status = set_val(status, SSTATUS_SPP, cpu.privilege);
    status = set_val(status, SSTATUS_SIE, 0);
    set_csr(CSR_SSTATUS, status);
    set_csr(CSR_STVAL, s->trap.tval);
    set_priv(PRV_S);
  }else{
    tvec = get_csr(CSR_MTVEC);
    status = get_csr(CSR_MSTATUS);
    rtlreg_t seq_pc = tvec + (tvec & 1? 4*cause : 0);
    rtl_j(s, seq_pc);
    set_csr(CSR_MCAUSE, cause);
    set_csr(CSR_MEPC, s->trap.pc);
    status = set_val(status, MSTATUS_MPIE, get_val(status, MSTATUS_MIE));
    status = set_val(status, MSTATUS_MPP, cpu.privilege);
    status = set_val(status, MSTATUS_MIE, 0);
    set_csr(CSR_MSTATUS, status);
    set_csr(CSR_MTVAL, s->trap.tval);
    set_priv(PRV_M);
  }
}
extern void (*ref_raise_intr)(uint64_t NO);
void timer_update();

void query_intr(DecodeExecState* s){
#ifndef AS_REF
  timer_update();
#endif
  rtlreg_t pending_int = get_csr(CSR_MIE) & get_csr(CSR_MIP);
  if(!pending_int) return;
  // printf("nemu raise intr: pc %lx cause %lx \n", cpu.pc, pending_int);
  rtlreg_t mstatus = get_csr(CSR_MSTATUS);
  rtlreg_t mideleg = get_csr(CSR_MIDELEG);
  rtlreg_t m_enable = cpu.privilege < PRV_M || (cpu.privilege == PRV_M && get_val(mstatus, MSTATUS_MIE));
  rtlreg_t enable_int = pending_int & ~mideleg & -m_enable; // must in M-mode
  if(!enable_int){
    rtlreg_t s_enable = cpu.privilege <= PRV_S && get_val(mstatus, MSTATUS_SIE);
    enable_int = mideleg & pending_int & -s_enable;
  }
//priority: MEI, MSI, MTI, SEI, SSI, STI
  if(enable_int){
    if(enable_int & MIP_MTIP){
      enable_int = MIP_MTIP;
      s->is_trap = 1;
      s->trap.cause = 7 | ((rtlreg_t)1 << 63);
      s->trap.pc = s->seq_pc;
      s->trap.tval = 0;
    }else if(enable_int & MIP_SSIP){
      s->is_trap = 1;
      s->trap.cause = 1 | ((rtlreg_t)1 << 63);
      s->trap.pc = s->seq_pc;
      s->trap.tval = 0;
    }else if(enable_int & MIP_STIP){
      enable_int = MIP_STIP;
      s->is_trap = 1;
      s->trap.cause = 5 | ((rtlreg_t)1 << 63);
      s->trap.pc = s->seq_pc;
      s->trap.tval = 0;
    }else{
      printf("enable int %lx\n", enable_int);
      assert(0);
    }
  }

}

vaddr_t isa_exec_once() {

  DecodeExecState s;
  s.is_jmp = 0;
  s.seq_pc = cpu.pc;
  s.is_trap = 0;
#ifndef AS_REF
  query_intr(&s);
#endif
  if(!s.is_trap){
    fetch_decode_exec(&s);
  }

  if(s.is_trap){
    s.is_trap = 0;
    take_trap(&s);
  }
  update_pc(&s);

  reset_zero();
  set_csr(CSR_INSTRET, get_csr(CSR_INSTRET) + 1);
  return s.seq_pc;
}

void isa_intr_exec(vaddr_t pc, word_t cause){
  // printf("nemu intr pc: %lx cause: %lx\n", pc, cause);
  DecodeExecState s;
  s.is_trap = 1;
  s.trap.pc = pc;
  s.trap.cause = cause;
  s.trap.tval = 0;

  take_trap(&s);
  update_pc(&s);
}