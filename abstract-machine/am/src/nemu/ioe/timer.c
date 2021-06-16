#include <am.h>
#include <nemu.h>

// static AM_TIMER_UPTIME_T _init_time_record;
void __am_timer_init() {
  //_init_time_record.us = (uint64_t)inl(RTC_ADDR+4) * 1000000 + inl(RTC_ADDR);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {

  uptime->us = 0; //(uint64_t)inl(RTC_ADDR+4) * 1000000 + inl(RTC_ADDR) - _init_time_record.us;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
