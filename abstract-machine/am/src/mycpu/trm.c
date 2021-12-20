#include <am.h>
#include <klib-macros.h>
#include <uart.h>

extern char _heap_start;
int main(const char *args);

extern char _addr_start;
#define PMEM_SIZE (128 * 1024 * 1024)
#define PMEM_END  ((uintptr_t)&_addr_start + PMEM_SIZE)

Area heap = RANGE(&_heap_start, PMEM_END);
#ifndef MAINARGS
#define MAINARGS ""
#endif
static const char mainargs[] = MAINARGS;

// #define FPGA

void putch(char ch) {
#ifdef FPGA
  if(ch == '\n') putch('\r');
  while((*((volatile uint8_t *)(UART_BASE + 8)) & (1 << 2)) == 0) ;
  *((volatile uint8_t *)(UART_BASE + 4)) = ch;
#else
  drv_uart_putc(ch);
#endif
}

void halt(int code) {
  // asm volatile("mv a0, %0; .word 0x0000006b" : :"r"(code));
  while (1);
}

void _trm_init() {
#ifdef FPGA
  *((volatile uint8_t *)(UART_BASE + 0xc)) = 3;
#else
  virt_uart_init();
  int ret = main(mainargs);
  halt(ret);
#endif
}
