#include <isa.h>
#include <memory/paddr.h>
#include <encoding.h>
#include <csr.h>
// this is not consistent with uint8_t
// but it is ok since we do not access the array directly
static const uint32_t img [] = {
  0x00000297,  //auipc t0, 0
  0x0002b823,  //sw zero, 16(t0)
  0x0102b503,  // lw a0, 16(t0)
  0x0000006b,  // nemu_trap
  0xdeadbeef,  //some data
};

void init_csr();

static void restart() {
  /* Set the initial program counter. */
#ifdef FLASH
  cpu.pc = 0x30000000;
#else
  #ifdef NANOS
    cpu.pc = 0x80100000;
  #else
    cpu.pc = 0x80000000;
  #endif
#endif

  /* The zero register is always 0. */
  cpu.gpr[0] = 0;
  cpu.privilege = M_MODE;
  init_csr();
}

void init_isa() {
  /* Load built-in image. */
  memcpy(guest_to_host(IMAGE_START), img, sizeof(img));

  /* Initialize this virtual computer system. */
  restart();
}


void init_csr(){
  cpu.csr[CSR_MSTATUS] = set_val(cpu.csr[CSR_MSTATUS], MSTATUS_SXL, 0b10);
  cpu.csr[CSR_MSTATUS] = set_val(cpu.csr[CSR_MSTATUS], MSTATUS_UXL, 0b10);
  cpu.csr[CSR_SSTATUS] = set_val(cpu.csr[CSR_SSTATUS], SSTATUS_UXL, 0b10);
  cpu.csr[CSR_MINSTRET] = 0;
  cpu.csr[CSR_MHARTID] = 0;
  cpu.csr[CSR_MISA] = 0x800000000014112dull;
  cpu.csr[CSR_USCRATCH] = 6;
}