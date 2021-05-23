#include <NDL.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t* keystate = NULL;
int screen_width = 0, screen_height = 0;

int SDL_Init(uint32_t flags) {
  printf("in init\n");
  keystate = (uint8_t*)malloc(256*sizeof(uint8_t));
  printf("before memset %lx %d\n", (uintptr_t)keystate);
  memset(keystate, 0, sizeof(keystate));
  printf("after memset\n");
  FILE* fp = fopen("/proc/dispinfo", "r");
  fscanf(fp, "WIDTH: %d\nHEIGHT: %d", &screen_width, &screen_height);
  fclose(fp);

  return NDL_Init(flags);
}

void SDL_Quit() {
  NDL_Quit();
}

char *SDL_GetError() {
  return "Navy does not support SDL_GetError()";
}

int SDL_SetError(const char* fmt, ...) {
  return -1;
}

int SDL_ShowCursor(int toggle) {
  // assert(0);
  return 0;
}

void SDL_WM_SetCaption(const char *title, const char *icon) {
}
