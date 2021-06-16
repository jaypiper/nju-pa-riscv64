#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

static void sh_handle_cmd(const char *cmd) {
  // char file_name[128] = {0};
  // strcpy(file_name, cmd);
  // file_name[strlen(file_name) - 1] = 0;
  // execvp(file_name, NULL);
  char file_name[128] = {0};
  strcpy(file_name, cmd);
  file_name[strlen(file_name) - 1] = 0;
  //查看是否为环境变量: 
  int i;
  for(i = 0; i < strlen(file_name); i++){
    if(file_name[i] == '=') {
      file_name[i] = 0;
      setenv(file_name, file_name+i+1, 0);
      return;
    }
  }
  //运行程序
  if(strncmp(file_name, "./", 2) == 0){
    setenv("PATH", "/bin:/user/bin", 0);
    int i;
    for(i = 2; i < strlen(file_name); i++){
      if(file_name[i] == ' '){
        // printf("%d %s\n", i, file_name);
        file_name[i] = 0;
        // printf("exe_file: %s\narg: %s\n", file_name+4, file_name+i+1);
        execl(file_name+1, file_name+i+1, NULL);
      }
    }
    printf("%s %s\n", file_name, file_name+1);
    execvp(file_name+1, NULL);
  }
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
