#include <isa.h>
#include <memory/paddr.h>

// this is not consistent with uint8_t
// but it is ok since we do not access the array directly
static const uint32_t img [] = {
  0x00000297,  //auipc t0, 0
  0x0002b823,  //sw zero, 16(t0)
  0x0102b503,  // lw a0, 16(t0)
  0x0000006b,  // nemu_trap
  0xdeadbeef,  //some data
};

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
}

void init_isa() {
  /* Load built-in image. */
  memcpy(guest_to_host(IMAGE_START), img, sizeof(img));

  /* Initialize this virtual computer system. */
  restart();
}
