CROSS_COMPILE := mips-linux-gnu-
COMMON_FLAGS  := -march=mips32 -fno-pic -fno-delayed-branch -mno-abicalls -mno-check-zero-division -EL
CFLAGS        += $(COMMON_FLAGS) -static -mno-llsc -mno-imadd -mno-mad
ASFLAGS       += $(COMMON_FLAGS) -O0
LDFLAGS       += -EL


AM_SRCS := mycpu-mips/start.S \
           mycpu-mips/trm.c \
           mycpu/ioe.c \
           mycpu/timer.c \
           mycpu/input.c \
           mycpu-mips/trap.S \
           mycpu/vme.c \
           mycpu/mpe.c \
           mycpu/uart.c

CFLAGS    += -fdata-sections -ffunction-sections
CFLAGS += -I$(AM_HOME)/am/src/mycpu/include
LDFLAGS   += -T $(AM_HOME)/scripts/platform/mycpu.ld --defsym=_stack_pointer=0x80100000 --defsym=_pmem_start=0x80000000
ifdef FLASH
    LDFLAGS += --defsym=_addr_start=0xbfc00000
else
    LDFLAGS += --defsym=_addr_start=0x80000000
endif
LDFLAGS   += --gc-sections -e _start
CFLAGS += -DMAINARGS=\"$(mainargs)\"
.PHONY: $(AM_HOME)/am/src/mycpu/trm.c

image: $(IMAGE).elf
	@$(OBJDUMP) -d $(IMAGE).elf > $(IMAGE).txt
	@echo + OBJCOPY "->" $(IMAGE_REL).bin
	$(OBJCOPY) -S --set-section-flags .bss=alloc,contents -O binary $(IMAGE).elf $(IMAGE).bin
