RV_ARCH       = rv64gc
CROSS_COMPILE := riscv64-linux-gnu-
COMMON_FLAGS  := -fno-pic -march=$(RV_ARCH) -mabi=lp64 -mcmodel=medany
CFLAGS        += $(COMMON_FLAGS) -static -Wno-sign-compare -Wno-maybe-uninitialized
ASFLAGS       += $(COMMON_FLAGS) -O0
LDFLAGS       += -melf64lriscv

AM_SRCS := mycpu/start.S \
           mycpu/trm.c \
           mycpu/libgcc/muldi3.S \
           mycpu/libgcc/div.S \
           mycpu/ioe.c \
           mycpu/timer.c \
           mycpu/input.c \
           mycpu/cte.c \
           mycpu/trap.S \
           mycpu/vme.c \
           mycpu/mpe.c \
           mycpu/uart.c

CFLAGS    += -fdata-sections -ffunction-sections
CFLAGS += -I$(AM_HOME)/am/src/mycpu/include
LDFLAGS   += -T $(AM_HOME)/scripts/platform/mycpu.ld --defsym=_pmem_start=0x80000000
ifdef FLASH
    LDFLAGS += --defsym=_addr_start=0x30000000
else ifdef LOADER
    LDFLAGS += --defsym=_addr_start=0x81000000
else
    LDFLAGS += --defsym=_addr_start=0x80000000
endif
LDFLAGS   += --gc-sections -e _start
CFLAGS += -DMAINARGS=\"$(mainargs)\"
.PHONY: $(AM_HOME)/am/src/mycpu/trm.c

image: $(IMAGE).elf
	@$(OBJDUMP) -d $(IMAGE).elf > $(IMAGE)-$(RV_ARCH).txt
	@echo + OBJCOPY "->" $(IMAGE_REL).bin
	@$(OBJCOPY) -S --set-section-flags .bss=alloc,contents -O binary $(IMAGE).elf $(IMAGE)-$(RV_ARCH).bin

run: image
	$(MAKE) -C $(NEMU_HOME) ISA=$(ISA) run ARGS="$(NEMUFLAGS)"