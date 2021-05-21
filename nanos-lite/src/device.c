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
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
