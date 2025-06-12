#include <stdint.h>
#include <uart.h>
#include <am.h>

void virt_uart_init(void)
{
    //http://byterunner.com/16550.html
    uart_write_reg(IER, 0x00);

    uint8_t lcr = uart_read_reg(LCR);
    uart_write_reg(LCR, lcr | (1 << 7));
    uart_write_reg(DLL, 13);
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


void sifive_uart_putc(char ch)
{
	while (sifive_get_reg(UART_REG_TXFIFO) & UART_TXFIFO_FULL)
		;

	sifive_set_reg(UART_REG_TXFIFO, ch);
}

void sifive_uart_init(unsigned long base, uint32_t in_freq, uint32_t baudrate)
{
	// uart_base     = (volatile char *)base;
	// uart_in_freq  = in_freq;
	// uart_baudrate = baudrate;

	/* Configure baudrate */
	// if (in_freq && baudrate)
	// 	sifive_set_reg(UART_REG_DIV, uart_min_clk_divisor(in_freq, baudrate));

	/* Disable interrupts */
	sifive_set_reg(UART_REG_IE, 0);

	/* Enable TX */
	sifive_set_reg(UART_REG_TXCTRL, UART_TXCTRL_TXEN);

	/* Enable Rx */
	sifive_set_reg(UART_REG_RXCTRL, UART_RXCTRL_RXEN);

}

void uartlite_init() {
  uartlite_set_reg(UART_LITE_CTRL_REG, UART_LITE_RST_FIFO);
}

void uartlite_putchar(char ch) {
  if (ch == '\n') uartlite_putchar('\r');

  while (uartlite_get_reg(UART_LITE_STAT_REG) & UART_LITE_TX_FULL);
  uartlite_set_reg(UART_LITE_TX_FIFO, ch);
}

int uartlite_getchar() {
  if (uartlite_get_reg(UART_LITE_STAT_REG) & UART_LITE_RX_VALID)
    return (int8_t)uartlite_get_reg(UART_LITE_RX_FIFO);
  return -1;
}