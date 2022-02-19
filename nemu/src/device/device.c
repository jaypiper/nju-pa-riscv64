#include <common.h>

#ifdef HAS_IOE

#include <device/alarm.h>
#include <device/vga.h>
#include <SDL2/SDL.h>
#include <monitor/monitor.h>

void init_alarm();
void init_serial();
void init_timer();
void init_vga();
void init_i8042();
void init_audio();
void init_plic();
void init_virtio();
void init_sdcard();
void* get_vmem();

void send_key(uint8_t, bool);
void vga_update_screen();

void send_uart(uint8_t scancode, bool is_keydown);
void timer_update();

#ifdef USE_NVBOARD
void nvboard_init(uint32_t* vmem, int is_mode800);
void nvboard_update();
uint8_t nvboard_get_key(bool* succ);
#else
static int device_update_flag = false;

static void set_device_update_flag() {
  device_update_flag = true;
}
#endif

void device_update() {
#ifdef USE_NVBOARD
  nvboard_update();
  bool succ = true;
  uint8_t k = nvboard_get_key(&succ);
  if(succ){
    bool is_keydown = nvboard_get_key(&succ);
    send_key(k, is_keydown);
    send_uart(k, is_keydown);
  }
#else
  if (!device_update_flag) {
    return;
  }
  device_update_flag = false;
  vga_update_screen();
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        nemu_state.state = NEMU_QUIT;
        break;
      // If a key was pressed
      case SDL_KEYDOWN:
      case SDL_KEYUP: {
        uint8_t k = event.key.keysym.scancode;
        bool is_keydown = (event.key.type == SDL_KEYDOWN);
        send_key(k, is_keydown);
        send_uart(k, is_keydown);
        break;
      }
      default: break;
    }
  }
#endif
}

void sdl_clear_event_queue() {
  SDL_Event event;
  while (SDL_PollEvent(&event));
}

void init_device() {
  init_serial();
  init_timer();
  init_vga();
  init_i8042();
#ifdef USE_NVBOARD
  void* vmem_addr = get_vmem();
  printf("vmem %lx\n", (uintptr_t)vmem_addr);
  #ifdef MODE_800x600
    nvboard_init(vmem_addr, 1);
  #else
    nvboard_init(vmem_addr, 0);
  #endif
#else
  init_audio();
  add_alarm_handle(set_device_update_flag);
  init_alarm();
#endif
  init_plic();
  init_virtio();
  init_sdcard();
}

#else

void init_device() {
}

#endif	/* HAS_IOE */
