#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

uint32_t NDL_GetTicks() {
  printf("here\n");
  struct timeval tv;
  gettimeofday(&tv, NULL);
  printf("finidhed %d\n", tv.tv_usec/1000);
  return tv.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  FILE* fp = fopen("/dev/events", "r");
  assert(fp);
  memset(buf, 0, len);
  // int fd = fileno(fp); 
  fgets(buf, len, fp);
  buf[strlen(buf) - 1] = 0;

  fclose(fp);
  return strlen(buf);
}

void NDL_OpenCanvas(int *w, int *h) {
  
  FILE* fp = fopen("/proc/dispinfo", "r");

  fscanf(fp, "WIDTH: %d\nHEIGHT: %d", &screen_w, &screen_h);
  // printf("%d %d\n", screen_w, screen_h);
  fclose(fp);
  if(*w == 0 && *h == 0){
    *w = screen_w; *h = screen_h;
  }

  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
   FILE* fp = fopen("/dev/fb", "w");
  int fd = fileno(fp);
  for(int i = 0; i < h; i++){
    // printf("line %d %d %x\n", i, (y+i)*screen_w+x, *pixels+ i*w);
    lseek(fd, ((y+i)*screen_w+x), SEEK_SET);
    // printf("seek done\n");
    write(fd, pixels + i * w, w);
  }
  fclose(fp);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
