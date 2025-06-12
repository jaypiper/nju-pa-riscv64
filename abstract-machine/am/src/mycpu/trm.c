#include <am.h>
#include <klib-macros.h>
#include <stdint.h>
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

void putch(char ch) {
  // drv_uart_putc(ch);
  sifive_uart_putc(ch);
  // uartlite_putchar(ch);
}

void halt(int code) {
  // asm volatile("mv a0, %0; .word 0x0000006b" : :"r"(code));
  while (1);
}

void _trm_init() {
  virt_uart_init();
  sifive_uart_init(UART_BASE, 0, 0);
  // uartlite_init();
  int ret = main(mainargs);
  halt(ret);
}
