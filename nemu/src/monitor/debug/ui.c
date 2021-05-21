#include <isa.h>
#include "expr.h"
#include "watchpoint.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/paddr.h>

void cpu_exec(uint64_t);
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_x(char *args){  
  char* N = strtok(args, " ");
  int n_val = atoi(N);
  char* EXPR = N + strlen(N) + 1;
  bool success = false;
  paddr_t _addr = expr(EXPR, &success);
  if(!_addr) printf("wrong address format\n");
  int i = 1;
  for( ; i <= n_val; i ++){
    if(i % 4 == 1) printf("%08x: ", _addr);
    printf("%08x", *((uint32_t*)paddr_read(_addr, 4)));
    _addr += 4;
    if(i % 4 == 0) printf("\n");
    else printf(" ");
  }
  return 0;
}

static int cmd_si(char* args){
  int num = 1;
  if(args) num = atoi(args);
  cpu_exec(num);
  return 0;
}

static int cmd_p(char* args){
  bool success = 0;
  word_t pval = expr(args, &success);
  if(!success) printf("wrong expression\n");
  // assert(success);
  printf("%lx  %lu\n", pval, pval);
  return 0;
}

static int cmd_w(char* args){
  WP* _wp = new_wp(args);
  printf("Watchpoint %d  %s\n", _wp->NO, _wp->watch_inst);
  return 0;
}

static int cmd_d(char* args){
  WP* selected = find_wp(atoi(args));
  free_wp(selected);
  return 0;
}

static int cmd_info(char *args){
  if(strcmp(args, "r") == 0) isa_reg_display();
  else if(strcmp(args, "w") == 0) watchpoint_info();
  else assert(0);
  return 0;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "info", "Display infomation about registers (info r) or watchpoints (info w)", cmd_info},
  { "x", "Display the next N 4B starting from EXPR. Usage (x N EXPR)", cmd_x},
  { "si", "Execute one instruction", cmd_si},
  { "p", "Evaluate a given expression", cmd_p},
  { "w", "Set a watchpoint", cmd_w},
  { "d", "Delete a watchpoint", cmd_d}
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop() {
  #ifndef DEBUG
  if (is_batch_mode()) {
    cmd_c(NULL);
    return;
  }
  #endif

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
