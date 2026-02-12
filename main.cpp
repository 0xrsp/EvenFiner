#include "cliargs.cpp"
#include "timer.cpp"

static bool win32_CreateProcess(s8* exe, s8* args, u64* out_time) {
  s8 cwd[MAX_PATH];
  GetCurrentDirectoryA(MAX_PATH, cwd);
  STARTUPINFOA start_info{};
  start_info.dwFlags = STARTF_USESTDHANDLES;
  PROCESS_INFORMATION proc_info{};

  if (!CreateProcessA(exe, args, nullptr, nullptr, 0, 0, nullptr, cwd, &start_info, &proc_info))
  {
    printf("unable to start process exe:%s args:%s\n", exe, args);
    return 0;
  }

  if (out_time) {
    *out_time = GetHighResTimestamp();
  }

  WaitForSingleObject(proc_info.hProcess, INFINITE);

  if (out_time) {
    *out_time = GetHighResTimestamp() - *out_time;
  }

  CloseHandle(start_info.hStdError);
  CloseHandle(start_info.hStdInput);
  CloseHandle(start_info.hStdOutput);

  CloseHandle(proc_info.hThread);
  CloseHandle(proc_info.hProcess);

  return 1;
}

#define FMT_TIME_UNITS(time,ms) ms ? time*1000.0 : time, ms ? "ms" : "s"

static void CompileResults(u64* timestamps, size_t N) {
  u64 total_ticks = 0;
  u64 min_ticks = 0xFF'FF'FF'FF'FF'FF'FF'FF, max_ticks = 0;
  for (size_t i = 0; i < N; ++i) {
    total_ticks += timestamps[i];
    min_ticks = min(min_ticks, timestamps[i]);
    max_ticks = max(max_ticks, timestamps[i]);
  }
  u64 avg_ticks = total_ticks / N;
  f64 avg_secs = avg_ticks / (f64)clock_freq;
  f64 min_secs = min_ticks / (f64)clock_freq;
  f64 max_secs = max_ticks / (f64)clock_freq;

  s64 sigma = 0;
  for (size_t i = 0; i < N; ++i) {
    s64 delta = timestamps[i] - avg_ticks;
    sigma += delta * delta;
  }
  f64 sd_ticks = sqrt(sigma / (f64)(N - 1));
  f64 sd_secs = sd_ticks / (f64)clock_freq;

  bool32 use_ms = avg_secs < 1.0;
  printf("Time\tmean:%f%s\tstddev:%f%s\n",
    FMT_TIME_UNITS(avg_secs, use_ms),
    FMT_TIME_UNITS(sd_secs, use_ms));

  printf("Range\t min:%f%s\tmax:%f%s\n",
    FMT_TIME_UNITS(min_secs, use_ms),
    FMT_TIME_UNITS(max_secs, use_ms));
}

s32 main(s32 argc, s8* argv[]) {
  InitHighResTimer();

  Cli_Args args{};
  ParseArgs(&args, argc, argv);
  //NOTE: Debug arg
  //args.directory = ".";

  u64* timestamps = (u64*)malloc(args.runs * sizeof(u64));

  for (s32 i = 0; i < args.warmup_runs; ++i) {
    if (!win32_CreateProcess(args.file, args.passed, nullptr)) {
      goto exit_err;
    }
  }

  for (s32 i = 0; i < args.runs; ++i) {
    u64 run_time = 0;
    if (!win32_CreateProcess(args.file, args.passed, &run_time)) {
      goto exit_err;
    }
    timestamps[i] = run_time;
  }

  CompileResults(timestamps, args.runs);
  return 0;
exit_err:
  return 1;
}
