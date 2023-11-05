#include <device/map.h>
#include <isa.h>
#include <csr.h>
#include "../isa/riscv64/local-include/reg.h"

/* http://en.wikibooks.org/wiki/Serial_Programming/8250_UART_Programming */
// NOTE: this is compatible to 16550

#define SERIAL_PORT 0x3F8
#define SERIAL_MMIO 0xa10003F8

#define CH_OFFSET 0

#define UART_BASE            (0x10000000L)

#define RHR 0    // Receive Holding Register (read mode)
#define THR 0    // Transmit Holding Register (write mode)
#define DLL 0    // LSB of Divisor Latch (write mode)
#define IER 1    // Interrupt Enable Register (write mode)
#define DLM 1    // MSB of Divisor Latch (write mode)
#define FCR 2    // FIFO Control Register (write mode)
#define ISR 2    // Interrupt Status Register (read mode)
#define LCR 3    // Line Control Register
#define MCR 4    // Modem Control Register
#define LSR 5    // Line Status Register
#define MSR 6    // Modem Status Register
#define SPR 7    // ScratchPad Register

static uint8_t *serial_base = NULL;
static uint8_t *uart_base = NULL;
static char uart_map[256] ={
' ', ' ', ' ', ' ', //0-3
'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',  //10
'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', // 1d
'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', //27
'\n'
};

static void serial_io_handler(uint32_t offset, int len, bool is_write) {
  assert(len == 1);
  switch (offset) {
    /* We bind the serial port with the host stderr in NEMU. */
    case CH_OFFSET:
      if (is_write) putc(serial_base[0], stderr);
      else panic("do not support read");
      break;
    default: panic("do not support offset = %d", offset);
  }
}

static int receive = 0;
static void uart_handler(uint32_t offset, int len, bool is_write){
  assert(len == 1);
  switch (offset) {
    /* We bind the serial port with the host stderr in NEMU. */
    case THR:
      if(is_write) putc(uart_base[THR], stderr);
      break;
    case LSR:
      uart_base[LSR] &= ~1;
      if(receive) {
        uart_base[LSR] |= 1;
        receive = 0;
      }
      break;
  }
}

void update_uart(){

}

void send_uart(uint8_t scancode, bool is_keydown){
  uart_base[RHR] = uart_map[scancode];
  if(is_keydown){
    receive = 1;
  }
  else{
    receive = 0;
  }
}


void init_serial() {
  // serial_base = new_space(8);
  // add_pio_map("serial", SERIAL_PORT, serial_base, 8, serial_io_handler);
  // add_mmio_map("serial", SERIAL_MMIO, serial_base, 8, serial_io_handler);

  uart_base = new_space(8);
  uart_base[5] = 0x20;
  add_mmio_map("uart", UART_BASE, uart_base, 8, uart_handler);
}
