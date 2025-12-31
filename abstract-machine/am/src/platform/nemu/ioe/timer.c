#include <am.h>
#include <nemu.h>

void __am_timer_init() {}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  // 根据NEMU设备文档,RTC寄存器由两个32位寄存器组成
  // 注意：必须先读高32位，这样才能触发时间更新
  // 然后再读低32位，保证读取的是同一时刻的值
  uint32_t hi = inl(RTC_ADDR + 4);        // 先读取高32位，触发时间更新
  uint32_t lo = inl(RTC_ADDR);            // 再读取低32位
  uptime->us = ((uint64_t)hi << 32) | lo; // 组合成64位微秒数
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour = 0;
  rtc->day = 0;
  rtc->month = 0;
  rtc->year = 1900;
}
