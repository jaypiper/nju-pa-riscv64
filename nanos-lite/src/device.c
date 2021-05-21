#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  // yield();
  const char* _p = buf;
  for(int i = 0; i < len; i++) putch(*_p++);
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  // yield();
  AM_INPUT_KEYBRD_T _keyboard = io_read(AM_INPUT_KEYBRD);
  if(_keyboard.keycode == AM_KEY_NONE) {
    sprintf(buf, "\n");
    return 1;
    // *(char*)buf = 0;
    // return 0;
  }
  if(_keyboard.keydown) sprintf(buf, "kd %s\n", keyname[_keyboard.keycode]);
  else sprintf(buf, "ku %s\n", keyname[_keyboard.keycode]);
  // printf("event: %s", buf);
  return strlen(buf);
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T _config = io_read(AM_GPU_CONFIG);
  // char tem_buf[128];
  return sprintf(buf, "WIDTH: %d\nHEIGHT: %d", _config.width, _config.height);
  printf("info %d %d\n", _config.width, _config.height);
  // strncpy(buf, tem_buf, len);
  // return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  // yield();
  AM_GPU_CONFIG_T _config = io_read(AM_GPU_CONFIG);
  
  int x = (offset/4) % _config.width;
  int y = (offset/4) / _config.width;
  // printf("x: %d, y: %d len: %d offset: %d\n", x, y, len, offset);
  // int left = _config.width - (offset % _config.width);
  // size_t i = 0;
  assert(offset + len <= _config.width * _config.height * 4);
  io_write(AM_GPU_FBDRAW, x, y, (void*)buf, len / 4, 1, true);
  // const uint32_t* pixels = buf;
  // while(i < len){
  //   // _fbdraw.h = 1;
  //   pixels = pixels + i;
  //   int w = _config.width - (offset  % _config.width);
  //   w = i + w < len ? w : len - i;
  //   io_write(AM_GPU_FBDRAW, x, y, (void*)pixels, w, 1, true);
  //   x += w;
  //   i += w;
  //   if(x == _config.width) {
  //     x = 0; y ++;
  //   }
  // }
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
