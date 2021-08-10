#include <memory.h>
#include <proc.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void* head = pf;
  pf += nr_page * PGSIZE;
  memset(head, 0, nr_page*PGSIZE);
  return head;
}

static inline void* pg_alloc(int n) {
  void* ret = new_page((n + PGSIZE - 1) / PGSIZE); // 如果没有对n的假设就 (n + PGSIZE - 1) / PGSIZE
  memset(ret, 0, n);
  return ret;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) { //max_brk表示下一页开头
#ifdef HAS_VME
  // printf("pre brk: %lx, new_brk: %lx\n", current->max_brk, brk);
  if(current->max_brk >= brk) return 0;
  while(current->max_brk < brk){
    void* _paddr = new_page(1);
    // printf("pa: %lx\n", (uintptr_t)_paddr);
    map(&current->as, (void*)current->max_brk, _paddr, 0);
    current->max_brk += PGSIZE;
  }
#endif
  // printf("brk: %x\n", current->max_brk);
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE); //按pgsize对齐
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
