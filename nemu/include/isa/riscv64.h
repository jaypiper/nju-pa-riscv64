#ifndef __ISA_RISCV64_H__
#define __ISA_RISCV64_H__

#include <common.h>

// memory
#ifdef NANOS
  #define riscv64_IMAGE_START 0x100000
#else
  #define riscv64_IMAGE_START 0
#endif

#define riscv64_PMEM_BASE 0x80000000

// reg

typedef struct {
  word_t gpr[32];
  word_t pc;
  word_t csr[0x1000];
  word_t privilege;
  bool INTR;
  word_t pre_lr;
  bool pre_lr_valid;
} riscv64_CPU_state;

// decode
typedef struct {
  union {
    struct{
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      uint32_t funct7    : 7;
    }r; // 寄存器-寄存器
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      int32_t  simm11_0  :12;
    } i; //短立即数和访存load
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t imm4_0    : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      int32_t  simm11_5  : 7;
    } s; //用于访存store 
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t imm11     : 1;
      uint32_t imm4_1    : 4;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      uint32_t imm10_5   : 6;
      int32_t  simm12    : 1;
    }b; //条件跳转
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      int32_t  simm31_12 :20;
    } u; //长立即数
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t simm19_12 : 8;
      uint32_t simm11    : 1;
      uint32_t simm10_1  : 10;
      int32_t  simm20    : 1;
    }j; //无条件跳转
    uint32_t val;
  } instr;
} riscv64_ISADecodeInfo;

typedef union{
  struct{
    uint16_t op     : 2;
    uint16_t rs2    : 5;
    uint16_t rs1    : 5;
    uint16_t funct4 : 4;
  }cr; // register
  struct{
    uint16_t op     : 2;
    uint16_t imm5   : 5;
    uint16_t rs1    : 5;
    int16_t imm1    : 1;
    uint16_t funct3 : 3;
  }ci; // immediate
  struct{
    uint16_t op     : 2;
    uint16_t rs2    : 5;
    uint16_t imm6   : 6;
    uint16_t funct3 : 3;
  }css; // stack relative store
  struct{
    uint16_t op     : 2;
    uint16_t rd_3   : 3;
    uint16_t imm8   : 8;
    uint16_t funct3 : 3;
  }ciw; // wide immediate
  struct{
    uint16_t op     : 2;
    uint16_t rs2_3  : 3;
    uint16_t imm2   : 2;
    uint16_t rs1_3  : 3;
    uint16_t imm3   : 3;
    uint16_t funct3 : 3;
  }cls; // load & store
  struct{
    uint16_t op     : 2;
    uint16_t imm5   : 5;
    uint16_t rs1_3  : 3;
    uint16_t imm3   : 3;
    uint16_t funct3 : 3;
  }cb; // branch
  struct{
    uint16_t op     : 2;
    uint16_t imm11  : 11;
    uint16_t funct3 : 3;
  }cj; //jump
  uint16_t val;
} riscv64c_IDInfo;
#ifndef VME
#define isa_vaddr_check(vaddr, type, len) (MEM_RET_OK)
#endif

#define riscv64_has_mem_exception() (cpu.mem_exception != 0)

#endif
