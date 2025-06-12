#ifndef UART_H
#define UART_H

#define UART_BASE            (0x10000000L)
// #define UART_BASE    (0x40600000L)

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

/* sifive uart */

#define UART_REG_TXFIFO		0
#define UART_REG_RXFIFO		1
#define UART_REG_TXCTRL		2
#define UART_REG_RXCTRL		3
#define UART_REG_IE		4
#define UART_REG_IP		5
#define UART_REG_DIV		6

#define UART_TXFIFO_FULL	0x80000000
#define UART_RXFIFO_EMPTY	0x80000000
#define UART_RXFIFO_DATA	0x000000ff
#define UART_TXCTRL_TXEN	0x1
#define UART_RXCTRL_RXEN	0x1

#define SIFIVE_UART_REG(reg) ((volatile unsigned int *)(UART_BASE + reg * 4))
#define sifive_get_reg(reg) (*(SIFIVE_UART_REG(reg)))
#define sifive_set_reg(reg, v) (*(SIFIVE_UART_REG(reg)) = (v))

void sifive_uart_init(unsigned long base, uint32_t in_freq, uint32_t baudrate);
void sifive_uart_putc(char ch);
/* xillix */
#define UART_LITE_RX_FIFO    0x0
#define UART_LITE_TX_FIFO    0x4
#define UART_LITE_STAT_REG   0x8
#define UART_LITE_CTRL_REG   0xc

#define UART_LITE_RST_FIFO   0x03
#define UART_LITE_INTR_EN    0x10
#define UART_LITE_TX_FULL    0x08
#define UART_LITE_TX_EMPTY   0x04
#define UART_LITE_RX_FULL    0x02
#define UART_LITE_RX_VALID   0x01

#define uartlite_get_reg(reg) (*(uint8_t*)(UART_BASE + reg))
#define uartlite_set_reg(reg, v) (*(uint8_t*)(UART_BASE + reg) = (v))
void uartlite_init();
void uartlite_putchar(char ch);
int uartlite_getchar();


#endif
