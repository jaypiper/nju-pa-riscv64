#include <isa.h>

void dev_raise_intr() {
#ifdef VME
    cpu.INTR = 1;
#endif
}
