#include <common.h>

void do_syscall(Context *c);
Context* schedule(Context *prev);

static Context* do_event(Event e, Context* c) {
  // printf("event %d\n", e.event);
  switch (e.event) {
    // case(EVENT_YIELD): printf("yield\n"); break;
    case(EVENT_YIELD): return schedule(c); break;
    case(EVENT_SYSCALL): do_syscall(c); break;
    case(EVENT_IRQ_TIMER): return schedule(c); break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
