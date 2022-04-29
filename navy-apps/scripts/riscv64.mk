CROSS_COMPILE = riscv64-linux-gnu-
LNK_ADDR = 0x2000000000
# LNK_ADDR = $(if $(VME), 0x40000000, 0x83000000)
CFLAGS  += -fno-pic -march=rv64g -mabi=lp64d -mcmodel=medany
LDFLAGS += --no-relax -Ttext-segment $(LNK_ADDR)
