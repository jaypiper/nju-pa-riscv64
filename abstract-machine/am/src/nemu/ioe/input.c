#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  kbd->keydown = 0;
  kbd->keycode = inl(KBD_ADDR);
  if(kbd->keycode & KEYDOWN_MASK) {
    kbd->keycode &= 0xfff;
    kbd->keydown = 1;
  }
}
