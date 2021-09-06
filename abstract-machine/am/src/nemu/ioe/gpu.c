#include <am.h>
#include <nemu.h>
#include<klib.h>
// #include <assert.h>
#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  // AM_GPU_CONFIG_T _config = io_read(AM_GPU_CONFIG);
  // int i;
  // int w = _config.width;  // TODO: get the correct width
  // int h = _config.height;  // TODO: get the correct height
  // uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  // for (i = 0; i < w * h; i ++) fb[i] = i;
  // outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t _wh = inl(VGACTL_ADDR);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = _wh >> 16, .height = _wh & 0xffff,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  AM_GPU_CONFIG_T _config = io_read(AM_GPU_CONFIG);
  int w = _config.width;  
  int h = _config.height;  
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for(int i = 0; i < ctl->h; i++){
    for(int j = 0; j < ctl->w; j++){
      // assert((i + ctl->y < h) && (j + ctl->x < w));
      if((i + ctl->y < h) && (j + ctl->x < w)){
        fb[w*(ctl->y+i)+ctl->x+j] = *((uint32_t*)ctl->pixels+i*ctl->w+j);
        // if(*((uint32_t*)ctl->pixels+i*ctl->w+j) != 0)
        // printf("nano addr: %x data: %x\n", &fb[w*(ctl->y+i)+ctl->x+j], *((uint32_t*)ctl->pixels+i*ctl->w+j));
      }
      // uintptr_t offset = (ctl->y + j) * 400 + (ctl->x + i);
      // printf("i:%d, j:%d x:%d y:%d w:%d h:%d %d \n",i, j,ctl->x,ctl->y, ctl->w, ctl->h, w*(ctl->y+i)+ctl->x+j);
      // outl(FB_ADDR + offset*4, *((uint32_t*)ctl->pixels+j*ctl->w+i));
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
