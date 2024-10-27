#include <assert.h>

#include "app_executor_interface.h"

BOOL
appOpenExecutor( AppExecutor *const executor ) {
  assert(executor != NULL);

  memset(executor, 0, sizeof(AppExecutor));

  SECURITY_ATTRIBUTES seqAttribs = {
    .nLength = sizeof(seqAttribs),
    .lpSecurityDescriptor = NULL,
    .bInheritHandle = TRUE,
  };

  if (!CreatePipe(&executor->_hStdinRead,  &executor->_hStdinWrite,  &seqAttribs, 512))
    return FALSE;

  if (!CreatePipe(&executor->_hStdoutRead, &executor->_hStdoutWrite, &seqAttribs, 512)) {
    CloseHandle(executor->_hStdinRead);
    CloseHandle(executor->_hStdinWrite);
    return FALSE;
  }

  executor->hStdin = executor->_hStdinWrite;
  executor->hStdout = executor->_hStdoutRead;

  STARTUPINFO startupInfo = {
    .cb = sizeof(startupInfo),
    .dwFlags = STARTF_USESTDHANDLES,

    .hStdInput = executor->_hStdinRead,
    .hStdOutput = executor->_hStdoutWrite,
    .hStdError = executor->_hStdoutWrite,
  };

  PROCESS_INFORMATION processInfo = {0};
  SECURITY_ATTRIBUTES processSecurityAttribs = {0};
  SECURITY_ATTRIBUTES threadSecurityAttribs = {0};

  TCHAR processName[512];

  GetModuleFileName(NULL, processName, sizeof(processName) / sizeof(TCHAR));

  TCHAR commandLine[] = L" -e";

  BOOL ok = CreateProcess(
    processName,
    commandLine,
    NULL,
    NULL,
    TRUE,
    0,
    NULL,
    NULL,
    &startupInfo,
    &processInfo
  );

  executor->_hProcess = processInfo.hProcess;
  executor->_hThread = processInfo.hThread;

  if (!ok) {
    // deinitialize stdin
    CloseHandle(executor->_hStdinRead);
    CloseHandle(executor->_hStdinWrite);

    // deinitialize stdout
    CloseHandle(executor->_hStdoutRead);
    CloseHandle(executor->_hStdoutWrite);
  }

  return ok;
} // appOpenExecutor function end

void
appCloseExecutor( AppExecutor *const executor ) {
  assert(executor != NULL);

  // deinitialize process
  if (executor->_hProcess != NULL) {
    CloseHandle(executor->_hProcess);
    CloseHandle(executor->_hThread);
  }

  // deinitialize stdin
  if (executor->_hStdinRead != NULL) {
    CloseHandle(executor->_hStdinRead);
    CloseHandle(executor->_hStdinWrite);
  }

  // deinitialize stdout
  if (executor->_hStdoutRead != NULL) {
    CloseHandle(executor->_hStdoutRead);
    CloseHandle(executor->_hStdoutWrite);
  }
} // appCloseExecutor function end

// app_daemon_executor.cpp file end