#include "pch.hpp"

#define CLI_DEFAULT_RUNS 5
#define CLI_DEFAULT_WARMUP 3

struct Cli_Args {
  s8* file = nullptr;
  s8* passed = nullptr;
  s32 warmup_runs = CLI_DEFAULT_WARMUP;
  s32 runs = CLI_DEFAULT_RUNS;
};

static void PrintOptionsHelp() {
  printf("Options help:\n");
  printf("\t-w<num>\tAmount of warmup runs (default:%d)\n", CLI_DEFAULT_WARMUP);
  printf("\t-r<num>\tAmount of runs (default:%d)\n", CLI_DEFAULT_RUNS);
}

static void PrintUsage(const s8* exe_name, bool32 opts_help = 1) {
  printf("%s [-Options] <exe_file> \"<args>\"\n", exe_name);
  if (opts_help) {
    PrintOptionsHelp();
  }
}

static bool32 IsAsciiDigit(s8 c) {
  return c >= '0' && c <= '9';
}

static void ParseArgs(Cli_Args* args, s32 argc, s8* argv[]) {
  for (s32 i = 1; i < argc; ++i) {
    s8* arg = argv[i];
    if (arg[0] == '-') {
      s8* arg1 = arg + 1;
      if (arg1[0] == 'w' && IsAsciiDigit(arg1[1])) {
        s32 warmups = atoi(arg1 + 1);
        warmups = max(0, warmups);
        args->warmup_runs = warmups;
        continue;
      }
      else if (arg1[0] == 'r' && IsAsciiDigit(arg1[1])) {
        s32 runs = atoi(arg1 + 1);
        runs = max(1, runs);
        args->runs = runs;
        continue;
      }
      else {
        printf("Unrecognized option: %s. Assuming it is the <args>\n", arg);
        //PrintOptionsHelp();
      }
    }

    if (args->file) {
      if (args->passed) {
        printf("Invalid argument: %s\n", arg);
        PrintUsage(argv[0], 0);
        exit(1);
      }
      args->passed = _strdup(args->file);
      strcat(args->passed, " ");
      strcat(args->passed, arg);
    }
    else {
      args->file = arg;
    }
  }

  if (!args->file) {
    printf("Missing <file>\n");
    PrintUsage(argv[0]);
    exit(1);
  }

  if (!args->passed) {
    printf("Missing \"<args>\"\n");
    PrintUsage(argv[0]);
    exit(1);
  }
}



