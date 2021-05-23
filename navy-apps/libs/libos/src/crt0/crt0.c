#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;

void call_main(uintptr_t *args) {
  char *empty[] =  {NULL };
  environ = empty;
  // int argc = *args;
  // char** argv = (char**)(args + 1);
  // char** envp = (char**)(args + 1 + argc + 1);

  // printf("argc: %d\n", argc);
  // for(int i = 0; i < argc; i++){
  //   printf("argv: %d %s %p\n", i, argv[i], &argv[i]);
  // }
  // for(int j = 0; envp[j]; j++) printf("envp: %d %s\n", j, envp[j]);
  // printf("end\n");
  
  // exit(main(argc, argv, envp));
  exit(main(0, empty, empty));
  assert(0);
}
