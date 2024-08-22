#include "app.h"

#include <signal.h>

HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

static void
appSignalHandler( int signal ) {
  AppExecutorTaskStatus taskStatus = APP_EXECUTOR_TASK_STATUS_CRASHED;
  AppExecutorCrashReport crashReport = {
    .signal = signal,
  };

  WriteFile(hStdout, &taskStatus, sizeof(taskStatus), NULL, NULL);
  WriteFile(hStdout, &crashReport, sizeof(crashReport), NULL, NULL);
} // appSignalHandler function end


int
appExecutorMain( int argc, const char **argv ) {
  printf("Executor started.\n");

  // setup signal handlers
  signal(SIGINT  , appSignalHandler);
  signal(SIGILL  , appSignalHandler);
  signal(SIGFPE  , appSignalHandler);
  signal(SIGSEGV , appSignalHandler);
  signal(SIGTERM , appSignalHandler);
  signal(SIGBREAK, appSignalHandler);
  signal(SIGABRT , appSignalHandler);

  *(int8_t *)NULL = 42;

  return 0;
} // appExecutorMain function end

// app_executor.cpp file end
