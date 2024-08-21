#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdint.h>

#include <windows.h>
#include <TlHelp32.h>

typedef enum __CliExecutionMode {
  CLI_EXECUTION_MODE_DAEMON,
  CLI_EXECUTION_MODE_CLIENT,
} CliExecutionMode;

#define INVALID_PROCESS_ID (~0U)

uint32_t findDaemonProcessId( const wchar_t *name ) {
  HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  uint32_t pid = INVALID_PROCESS_ID;

  PROCESSENTRY32 entry;
  entry.dwSize = sizeof(entry);

  do {
    if (lstrcmpW(entry.szExeFile, name) == 0) {
      pid = entry.th32ProcessID;
      break;
    }
  } while (Process32Next(handle, &entry));

  CloseHandle(handle);
  return pid;
}

int
cliRunClient( void ) {
  uint32_t daemonPid = findDaemonProcessId(L"ss_sqs.exe");
  if (daemonPid == INVALID_PROCESS_ID) {
    printf("Can't find daemon PID\n");
  } else {
    printf("Daemon PID: %d\n", daemonPid);
  }

  return 0;
}

int
cliRunDaemon( void ) {
  printf("PID: %d\n", GetCurrentProcessId());

  while (TRUE) {
    __noop;
  }

  return 0;
}

//----------------------------------------------------------------
//! @brief project entry point
//!
//! @param [in] argc count of console parameters
//! @param [in] argv parameter strings
//! 
//! @return exit status
//----------------------------------------------------------------
int
main(
  int argc,
  const char **argv
) {
  CliExecutionMode executionMode = CLI_EXECUTION_MODE_CLIENT;

  for (uint32_t argI = 1; argI < (uint32_t)argc; argI++) {
    if (strcmp(argv[argI], "-d") == 0) {
      executionMode = CLI_EXECUTION_MODE_DAEMON;
      continue;
    }

    if (strcmp(argv[argI], "-c") == 0) {
      executionMode = CLI_EXECUTION_MODE_CLIENT;
      continue;
    }
  }

  switch (executionMode) {
  case CLI_EXECUTION_MODE_DAEMON : return cliRunDaemon();
  case CLI_EXECUTION_MODE_CLIENT : return cliRunClient();
  default                        : return 1;
  }
} // main function end

// sqs_main.c file end
