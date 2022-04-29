LNK_ADDR = 0x100000000000
# CFLAGS  += -fno-pic -march=i386 -mstringop-strategy=loop -mno-inline-all-stringops
CFLAGS  +=  -mcmodel=large -fcf-protection=none # remove endbr32 in Ubuntu 20.04 with a CPU newer than Comet Lake
LDFLAGS += -Ttext-segment $(LNK_ADDR) --no-relax
