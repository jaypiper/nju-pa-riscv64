#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", arg, j);
    j ++;
    yield();
  }
}

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void (*entry)(void*), void* arg);
void context_uload(PCB* pcb, const char* filename, char *const argv[], char *const envp[]);


void init_proc() {
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  // naive_uload(NULL, "/bin/nterm");
  // context_kload(&pcb[1], hello_fun, "bb");
  // context_kload(&pcb[0], hello_fun, "aa");
  // context_uload(&pcb[1], "/bin/hello", NULL, NULL);
  // char* argv[] = {
  //   // "--skip",
  //   // "hello",
  //   "/bin/exec-test",
  //   NULL
  // };
  // char* envp[] = {
  //   "envp1",
  //   "envp2",
  //   NULL
  // };
#ifdef HAS_VME
  // context_uload(&pcb[1], "/bin/pal", argv, envp);
  // context_uload(&pcb[0], "/bin/event-test", NULL, NULL);
  context_uload(&pcb[0], "/bin/pal", NULL, NULL);
  context_uload(&pcb[1], "/bin/bird", NULL, NULL);
  context_uload(&pcb[2], "/bin/nterm", NULL, NULL);
  context_uload(&pcb[3], "/bin/hello", NULL, NULL);
#else
  context_uload(&pcb[0], "/bin/pal", NULL, NULL);
#endif
  switch_boot_pcb();
  yield();
  // context_uload(&pcb[1], "/bin/nterm",argv , NULL);
}
static int next = 0;
Context* schedule(Context *prev) {
#ifdef HAS_VME
  assert(prev);
  static int i = 0;
  i++;
  current->cp = prev;
  if(current == &pcb_boot) {
    printf("leave boot\n");
    current = &pcb[0];
  }
  if(i % 100 == 0) current = &pcb[3];
  else{
      current = &pcb[next];
  }
#else
  current = &pcb[0];
#endif

  return current->cp;
}
void set_next_pcb(int id){
  next = id;
}