#include "../local-include/rtl.h"
#include <csr.h>
#include "compute.h"
#include "control.h"
#include "ldst.h"
#include "muldiv.h"
#include "system.h"
#include "atomic.h"
#include "compress.h"

def_EHelper(inv);
def_EHelper(nemu_trap);
def_EHelper(nemu_exit);
def_EHelper(fence);
def_EHelper(nop);
def_EHelper(illegal);