#ifndef __RISCV64_REG_H__
#define __RISCV64_REG_H__

#include <common.h>

#define SEPC_ID 0x141
#define STVEC_ID 0x105
#define SCAUSE_ID 0x142
#define STVAL_ID 0x143
#define SSCRATCH_ID 0x140
#define SSTATUS_ID 0x100
#define SATP_ID 0x180  

static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 32);
  return index;
}

static int all_scr[7] = {SEPC_ID, STVEC_ID, SCAUSE_ID, STVAL_ID, SSCRATCH_ID, SSTATUS_ID, SATP_ID};

static inline int get_scr_id(int id){
  for(int i = 0; i < 7; i++){
    if(all_scr[i] == id) return i;
  }
  printf("%d %x\n", id, id);
  assert(0);
}

#define reg_d(index) (cpu.gpr[check_reg_index(index)]._64)
#define reg_scr(id) (cpu.csr[get_scr_id(id)]._64)

static inline const char* reg_name(int index) {
  extern const char* regs[];
  assert(index >= 0 && index < 32);
  return regs[index];
}

#endif
