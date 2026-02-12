#include "pch.hpp"

static u64 clock_freq;

static void InitHighResTimer() {
  QueryPerformanceFrequency((LARGE_INTEGER*)&clock_freq);
}

static u64 GetHighResTimestamp() {
  u64 ticks_now;
  QueryPerformanceCounter((LARGE_INTEGER*)&ticks_now);
  return ticks_now;
}

