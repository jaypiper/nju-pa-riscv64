#include "../../include/common.h"
#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)
#define MAP(c, f) c(f)

#define REGS(f) \
      f( 1)       f( 3) f( 4) f( 5) f( 6) f( 7) f( 8) f( 9) \
f(10) f(11) f(12) f(13) f(14) f(15) f(16) f(17) f(18) f(19) \
f(20) f(21) f(22) f(23) f(24) f(25) f(26) f(27) f(28) f(29) \
f(30) f(31)

#define PUSH(n) sd concat(x, n), (n * 8)(sp);
#define POP(n)  ld concat(x, n), (n * 8)(sp);

#define CONTEXT_SIZE ((32 + 5) * 8)
#define OFFSET_SP     ( 2 * 8)
#define OFFSET_CAUSE  (32 * 8)
#define OFFSET_STATUS (33 * 8)
#define OFFSET_EPC    (34 * 8)
#define OFFSET_NP     (35 * 8)

.globl __am_asm_trap
__am_asm_trap:
#ifdef HAS_VME
  csrrw sp, sscratch, sp
  bnez sp, save_np_user
  csrr sp, sscratch
  j save_np_kernel

save_np_user:
  addi sp, sp, -CONTEXT_SIZE

  MAP(REGS, PUSH)

  li t1, 1
  sd t1, OFFSET_NP(sp)

  j save_context

save_np_kernel:
  addi sp, sp, -CONTEXT_SIZE
  MAP(REGS, PUSH)

  li t1, 0
  sd t1, OFFSET_NP(sp)

  j save_context

save_context:
  csrr t0, sscratch
  sd t0, OFFSET_SP(sp)

  csrwi sscratch, 0

  csrr t0, scause
  csrr t1, sstatus
  csrr t2, sepc

  sd t0, OFFSET_CAUSE(sp)
  sd t1, OFFSET_STATUS(sp)
  sd t2, OFFSET_EPC(sp)

  mv a0, sp
  jal __am_irq_handle
  mv sp, a0

  ld t1, OFFSET_STATUS(sp)
  ld t2, OFFSET_EPC(sp)
  csrw sstatus, t1
  csrw sepc, t2

  ld t1, OFFSET_NP(sp)
  bnez t1, am_recover_user
  MAP(REGS, POP)

  addi sp, sp, CONTEXT_SIZE

  sret

am_recover_user:
  MAP(REGS, POP)

  addi sp, sp, CONTEXT_SIZE
  csrw sscratch, sp
  addi sp, sp, -CONTEXT_SIZE
  ld sp, OFFSET_SP(sp)

  sret

#else

  addi sp, sp, -CONTEXT_SIZE

  MAP(REGS, PUSH)

  mv t0, sp
  addi t0, t0, CONTEXT_SIZE
  sd t0, OFFSET_SP(sp)

  csrr t0, scause
  csrr t1, sstatus
  csrr t2, sepc

  sd t0, OFFSET_CAUSE(sp)
  sd t1, OFFSET_STATUS(sp)
  sd t2, OFFSET_EPC(sp)

  mv a0, sp
  jal __am_irq_handle
  mv sp, a0

  ld t1, OFFSET_STATUS(sp)
  ld t2, OFFSET_EPC(sp)
  csrw sstatus, t1
  csrw sepc, t2

  MAP(REGS, POP)

  addi sp, sp, CONTEXT_SIZE

  sret

#endif
