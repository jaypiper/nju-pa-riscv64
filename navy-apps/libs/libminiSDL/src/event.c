#include <NDL.h>
#include <SDL.h>
#include <assert.h>

#define keyname(k) #k,

extern uint8_t* keystate;

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  assert(0);
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char buf[128];
  NDL_PollEvent(buf, sizeof(buf)-1) ;

  if(strncmp(buf, "kd", 2) == 0){
    ev->type = SDL_KEYDOWN;
    
  }
  else if(strncmp(buf, "ku", 2) == 0){
    ev->type = SDL_KEYUP;
  }
  else{
    return 0;
  }
  
  for(int i = 0; i < sizeof(keyname) / sizeof(char*); i++){
    // printf("%s  %s\n", buf+3, keyname[i]);
    if(strcmp(buf + 3, keyname[i]) == 0){
      ev->key.keysym.sym = i;
      if(ev->type == SDL_KEYDOWN) keystate[i] = 1;
      else keystate[i] = 0;
      return 1;
    }
  }
  assert(0);
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  char buf[128];
  while(!NDL_PollEvent(buf, sizeof(buf)-1)) ;

  if(strncmp(buf, "kd", 2) == 0){
    event->type = SDL_KEYDOWN;
  }
  else if(strncmp(buf, "ku", 2) == 0){
    event->type = SDL_KEYUP;
  }
  else{
    return 0;
  }
  
  for(int i = 0; i < sizeof(keyname) / sizeof(char*); i++){
    // printf("%s  %s\n", buf+3, keyname[i]);
    if(strcmp(buf + 3, keyname[i]) == 0){
      event->key.keysym.sym = i;
      return 1;
    }
  }
  
  assert(0);
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return keystate;
}
