#ifndef __RISCV64_REG_H__
#define __RISCV64_REG_H__

#include <common.h>
#include <encoding.h>

#define SEPC_ID       0x141
#define STVEC_ID      0x105
#define SCAUSE_ID     0x142
#define STVAL_ID      0x143
#define SSCRATCH_ID   0x140
#define SSTATUS_ID    0x100
#define SATP_ID       0x180
#define SIE_ID        0x104
#define SIP_ID        0x144
#define MTVEC_ID      0x305
#define MEPC_ID       0x341
#define MCAUSE_ID     0x342
#define MIE_ID        0x304
#define MIP_ID        0x344
#define MTVAL_ID      0x343
#define MSCRATCH_ID   0x340
#define MSTATUS_ID    0x300
#define MEDELEG_ID    0x302
#define MIDELEG_ID    0x303
#define MHARTID       0xf14

static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 32);
  return index;
}

static int all_scr[7] = {SEPC_ID, STVEC_ID, SCAUSE_ID, STVAL_ID, SSCRATCH_ID, SSTATUS_ID, SATP_ID};

static inline int get_scr_id(int id){
  return id;
  for(int i = 0; i < 7; i++){
    if(all_scr[i] == id) return i;
  }
  printf("%d %x\n", id, id);
  assert(0);
}

#define reg_d(index) (cpu.gpr[check_reg_index(index)])
// #define get_csr(id) (cpu.csr[id])

extern int csr_num;
extern const int csrs[];
extern const char* csr_name[];

static inline const char* reg_name(int index) {
  extern const char* regs[];
  assert(index >= 0 && index < 32);
  return regs[index];
}

void set_csr(int id, rtlreg_t val);
rtlreg_t get_csr(int id);
void set_priv(int priv);
word_t get_priv();
void set_csr_cond(DecodeExecState* s, int id, rtlreg_t val);
rtlreg_t get_csr_cond(DecodeExecState* s, int id);
rtlreg_t get_pre_lr();
bool pre_lr_valid();
void set_pre_lr(rtlreg_t pre_lr);
void clear_pre_lr();
#endif
