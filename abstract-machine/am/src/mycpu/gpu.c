#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#define FB_ADDR 0x7fe00000
#define VGA_CTRL 0x10003000
#define MAP_CTRL 0x10004000
#define VGA_BASE 0x81000000

void __am_gpu_init() {
  *(uint32_t*)(VGA_CTRL) = 0;
  *(uint32_t*)(VGA_CTRL + 4) = VGA_BASE;
  *(uint32_t*)(MAP_CTRL) = VGA_BASE - FB_ADDR;

  volatile uint32_t write_mode = *(uint32_t*)(VGA_CTRL);
  printf("mode: %x ", write_mode);
  volatile uint32_t write_base = *(uint32_t*)(VGA_CTRL + 4);
  printf("base: %x ", write_base);
  volatile uint32_t write_offset = *(uint32_t*)(MAP_CTRL);
  printf("offset: %x\n", write_offset);
  uint32_t* fb = (uint32_t*)VGA_BASE;
  for(int i = 0; i < 800*600; i++) fb[i] = 0;
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t vga_status = *(uint32_t*)(VGA_CTRL);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = vga_status? 400 : 800, .height = vga_status? 300 : 600,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  AM_GPU_CONFIG_T _config = io_read(AM_GPU_CONFIG);
  int w = _config.width;  
  int h = _config.height;  
  // printf("w: %d h: %d ctrl w %d  ctrl h %d\n", w, h, ctl->w, ctl->h);
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for(int i = 0; i < ctl->h; i++){
    for(int j = 0; j < ctl->w; j++){
      // printf("i: %d j: %d\n", i, j);
      if((i + ctl->y < h) && (j + ctl->x < w)){
        fb[w*(ctl->y+i)+ctl->x+j] = *((uint32_t*)ctl->pixels+i*ctl->w+j);
      }
    }
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
