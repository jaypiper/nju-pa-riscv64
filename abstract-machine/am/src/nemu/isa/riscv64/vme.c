#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)
#define VALID_MASK 1
#define PGTABLE_MASK 0x3ffffffffffc00


static inline void set_satp(void *pdir) {
  uintptr_t mode = 8ull << 60;
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
  
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  printf("begin vme init\n");
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }
  set_satp(kas.ptr);
  vme_enable = 1;
  printf("vme init done\n");
  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

void map(AddrSpace *as, void *va, void *pa, int prot) { //proc暂时不使用
  printf("map in space ptr: %p va: %p pa: %p\n", as->ptr, va, pa);
  assert(as->ptr);
  uint64_t* pg_base = (uint64_t*)as->ptr;
  uint64_t _vaddr = (uint64_t)va;
  uint64_t idx = (_vaddr >> 30) & 0x1ff;
  
  if(!(pg_base[idx] & VALID_MASK)){
    void* _new_pg = pgalloc_usr(PGSIZE);
    pg_base[idx] = ((uint64_t)_new_pg & PGTABLE_MASK) | VALID_MASK;
  }
  // printf("pg_base: %p idx: %x item: %x\n", pg_base, idx, pg_base[idx]);
  pg_base = (uint64_t*)pg_base[idx];
  idx = (_vaddr >> 21) & 0x1ff;
  if(!(pg_base[idx] & VALID_MASK)){
    void* _new_pg = pgalloc_usr(PGSIZE);
    pg_base[idx] = ((uint64_t)_new_pg & PGTABLE_MASK) | VALID_MASK;
  }
  // printf("pg_base: %p idx: %x item: %x\n", pg_base, idx, pg_base[idx]);
  pg_base = (uint64_t*)pg_base[idx];
  idx = (_vaddr >> 12) & 0x1ff;
  pg_base[idx] = ((uint64_t)pa & PGTABLE_MASK) |VALID_MASK;
  // printf("pg_base: %p idx: %x item: %x\n\n", pg_base, idx, pg_base[idx]);
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  // printf("end: %lx\n", kstack.end);
  Context* _context = (Context*)(kstack.end - (32+3)*8);
  // printf("context: %lx\n", (uintptr_t)_context);
  _context->pdir = as->ptr;
  _context->epc = (uintptr_t)entry;
  // printf("epc: %lx\n", (uintptr_t)entry);
  return _context;
}
