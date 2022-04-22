#include <am.h>
#include <klib.h>
#include <riscv64.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    #define MSG(m) ev.msg = m;
    #define IRQ(name) (((uintptr_t)1 << 63) | IRQ_ ## name)

    switch (c->cause) {
      case IRQ(M_TIMER): MSG("M-mode timer interrupt");
        ev.event = EVENT_IRQ_TIMER; break;
      case IRQ(S_SOFT): MSG("S-soft timer interrupt");
        ev.event = EVENT_IRQ_TIMER; break;
      case IRQ(S_EXT): MSG("S-mode ext interrupt");
        ev.event = EVENT_IRQ_IODEV; break;
      case CAUSE_FETCH_PAGE_FAULT: MSG("fetch page fault");
        ev.event = EVENT_PAGEFAULT; break;
      case CAUSE_LOAD_PAGE_FAULT: MSG("load page fault");
        ev.event = EVENT_PAGEFAULT; break;
      case CAUSE_STORE_PAGE_FAULT: MSG("store page fault");
        ev.event = EVENT_PAGEFAULT; break;
      case CAUSE_USER_ECALL: MSG("user ecall");
        ev.event = EVENT_SYSCALL; break;
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
    r_gpr("tp", c->gpr[4]);
  }
  return c;
}

extern void kernel_trap(void);


bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw stvec, %0" : : "r"(kernel_trap));

  // register event handler
  if(handler) user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
   Context *ctx = kstack.end - sizeof(Context);
  *ctx = (Context) { 0 };
  ctx->epc = (uintptr_t)entry;
  ctx->gpr[10] = (uintptr_t)arg;
  ctx->gpr[1] = (uintptr_t)kstack.end;
  ctx->kernel_satp = 0;
  ctx->kernel_trap = (uintptr_t)kernel_trap;

  return ctx;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  uintptr_t status;
  r_csr("sstatus", status);
  return status & SSTATUS_SIE;
}

void iset(bool enable) {
  uintptr_t status;
  r_csr("sstatus", status);
  if(enable) {w_csr("sstatus", status | SSTATUS_SIE);}
  else {w_csr("sstatus", status & (~ SSTATUS_SIE));}
}
