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
  context_kload(&pcb[1], hello_fun, "bb");
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
  // context_uload(&pcb[1], "/bin/pal", argv, envp);
  // context_uload(&pcb[0], "/bin/event-test", NULL, NULL);
  context_uload(&pcb[0], "/bin/pal", NULL, NULL);
  // context_uload(&pcb[1], "/bin/hello", NULL, NULL);
  switch_boot_pcb();
  yield();
  // context_uload(&pcb[1], "/bin/nterm",argv , NULL);
}

Context* schedule(Context *prev) {
  assert(prev);
  // current->cp = prev;
  // current = &pcb[0];
  // printf("after switch to 0\n");
  static int i = 0;
  i++;
  if(current == &pcb_boot) printf("leave boot\n");
  current->cp = prev;
  // printf("i: %d\n", i);
  if(current == &pcb[0] && (i % 100) == 0){
    current = &pcb[1];
    // printf("switch to 1\n");
  }
  else {
    current = &pcb[0];
    // printf("switch to 0\n");
  }
  // assert(pcb[0].cp);
  // assert(pcb[1].cp);
  // assert(current->cp);
  return current->cp;
}
