#include <device/map.h>
#include <device/alarm.h>
#include <monitor/monitor.h>
#include <sys/time.h>
#include <isa.h>
#include <csr.h>
#include "../isa/riscv64/local-include/reg.h"

#define RTC_PORT 0x48   // Note that this is not the standard
#define RTC_MMIO 0xa1000048
//ask the CLINT for a timer interrupt.
#define CLINT                   (0x2000000L)
#define CLINT_MTIMECMP(hartid)  (CLINT + 0x4000 + 4*(hartid))
#define CLINT_MTIME             (CLINT + 0xBFF8)            // cycles since boot.

static uint32_t *rtc_port_base = NULL;
static uint8_t  *clint_port = NULL;
static uint64_t  *clint_mtimecmp_port = NULL;
static uint64_t  *clint_mtime_port = NULL;

static void rtc_io_handler(uint32_t offset, int len, bool is_write) {
  assert(offset == 0 || offset == 4);
  if (!is_write) {
    struct timeval now;
    gettimeofday(&now, NULL);
    rtc_port_base[0] = now.tv_usec;
    rtc_port_base[1] = now.tv_sec;
  }
}

static void timer_intr() {
  if (nemu_state.state == NEMU_RUNNING) {
    extern void dev_raise_intr();
    dev_raise_intr();
  }
}

void (*ref_raise_intr)(uint64_t NO);
void (*ref_clear_mip)();

static void write_timer_cmp(uint32_t offset, int len, bool is_write){
  if(is_write){
    set_csr(CSR_MIP, set_val(get_csr(CSR_MIP), MIP_MTIP, 0));
    ref_clear_mip();
  }
}

static void write_timer(uint32_t offset, int len, bool is_write){
}


void timer_update(){
  *clint_mtime_port = *clint_mtime_port + 1;
  if(*clint_mtime_port > *clint_mtimecmp_port && (get_csr(CSR_MIP) & MIP_MTIP) == 0){
    set_csr(CSR_MIP, set_val(get_csr(CSR_MIP), MIP_MTIP, 1));
    ref_raise_intr(7);
  }
}

void init_timer() {
  rtc_port_base = (void*)new_space(8);
  add_pio_map("rtc", RTC_PORT, (void *)rtc_port_base, 8, rtc_io_handler);
  add_mmio_map("rtc", RTC_MMIO, (void *)rtc_port_base, 8, rtc_io_handler);
  add_alarm_handle(timer_intr);

  clint_port = new_space(1);
  add_mmio_map("clint", CLINT, clint_port, 1, NULL);
  clint_mtimecmp_port = (void*)new_space(0x8);
  add_mmio_map("clint_mtimecmp", CLINT_MTIMECMP(0), (void*)clint_mtimecmp_port, 8, write_timer_cmp);
  clint_mtime_port = (void*)new_space(8);
  add_mmio_map("clint_mtime", CLINT_MTIME, (void*)clint_mtime_port, 8, write_timer);
}
