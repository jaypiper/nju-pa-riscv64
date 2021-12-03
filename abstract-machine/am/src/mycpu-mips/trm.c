#include <am.h>
#include <klib-macros.h>
// #include <uart.h>

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


#define UART_BASE            (0xbfe40000)

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

#define UART_REG(reg) ((volatile uint8_t *)(UART_BASE + reg))

#define LSR_RX_READY (1 << 0)
#define LSR_TX_IDLE  (1 << 5)

#define uart_read_reg(reg) (*(UART_REG(reg)))
#define uart_write_reg(reg, v) (*(UART_REG(reg)) = (v))

void virt_uart_init(void);
int drv_uart_putc(char c);



void virt_uart_init(void)
{
    //http://byterunner.com/16550.html
    uart_write_reg(IER, 0x00);

    uint8_t lcr = uart_read_reg(LCR);
    uart_write_reg(LCR, lcr | (1 << 7));
    uart_write_reg(DLL, 14);
    uart_write_reg(DLM, 0x00);

    lcr = 0;
    uart_write_reg(LCR, lcr | (3 << 0));

    /*
     * enable receive interrupts.
     */
    uint8_t ier = uart_read_reg(IER);
    uart_write_reg(IER, ier | (1 << 0));
}

int drv_uart_putc(char c)
{
    while ((uart_read_reg(LSR) & LSR_TX_IDLE) == 0);
    return uart_write_reg(THR, c);
}

void putch(char ch) {
  drv_uart_putc(ch);
}

void halt(int code) {
//   asm volatile("mv a0, %0; .word 0x0000006b" : :"r"(code));
  while (1);
}

void _trm_init() {
  virt_uart_init();
  int ret = main(mainargs);
  halt(ret);
}
