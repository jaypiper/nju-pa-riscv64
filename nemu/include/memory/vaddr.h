#ifndef __MEMORY_VADDR_H__
#define __MEMORY_VADDR_H__

#include <common.h>
#include <cpu/decode.h>

static inline word_t vaddr_read(DecodeExecState* s, vaddr_t addr, int len) {
  word_t vaddr_read1(DecodeExecState* s, vaddr_t addr);
  word_t vaddr_read2(DecodeExecState* s, vaddr_t addr);
  word_t vaddr_read4(DecodeExecState* s, vaddr_t addr);
#ifdef ISA64
  word_t vaddr_read8(DecodeExecState* s, vaddr_t addr);
#endif
  switch (len) {
    case 1: return vaddr_read1(s, addr);
    case 2: return vaddr_read2(s, addr);
    case 4: return vaddr_read4(s, addr);
#ifdef ISA64
    case 8: return vaddr_read8(s, addr);
#endif
    default: assert(0);
  }
}

static inline void vaddr_write(DecodeExecState* s,vaddr_t addr, word_t data, int len) {
  void vaddr_write1(DecodeExecState* s, vaddr_t addr, word_t data);
  void vaddr_write2(DecodeExecState* s, vaddr_t addr, word_t data);
  void vaddr_write4(DecodeExecState* s, vaddr_t addr, word_t data);
#ifdef ISA64
  void vaddr_write8(DecodeExecState*s, vaddr_t addr, word_t data);
#endif
  switch (len) {
    case 1: vaddr_write1(s, addr, data); break;
    case 2: vaddr_write2(s, addr, data); break;
    case 4: vaddr_write4(s, addr, data); break;
#ifdef ISA64
    case 8: vaddr_write8(s, addr, data); break;
#endif
    default: assert(0);
  }
}

static inline word_t vaddr_ifetch(DecodeExecState* s, vaddr_t addr, int len) {
  word_t vaddr_ifetch1(DecodeExecState* s, vaddr_t addr);
  word_t vaddr_ifetch2(DecodeExecState* s, vaddr_t addr);
  word_t vaddr_ifetch4(DecodeExecState* s, vaddr_t addr);
#ifdef ISA64
  word_t vaddr_ifetch8(DecodeExecState* s, vaddr_t addr);
#endif
  switch (len) {
    case 1: return vaddr_ifetch1(s, addr);
    case 2: return vaddr_ifetch2(s, addr);
    case 4: return vaddr_ifetch4(s, addr);
#ifdef ISA64
    case 8: return vaddr_ifetch8(s, addr);
#endif
    default: assert(0);
  }
}


#define PAGE_SIZE         4096
#define PAGE_MASK         (PAGE_SIZE - 1)
#define PG_ALIGN __attribute((aligned(PAGE_SIZE)))

#endif
