#include <assert.h>

#include "app_executor_interface.h"

BOOL
appOpenExecutor( AppExecutor *const executor ) {
  assert(executor != NULL);

  memset(executor, 0, sizeof(AppExecutor));

  if (!CreatePipe(&executor->_hStdinRead,  &executor->_hStdinWrite,  NULL, 0))
    return FALSE;

  if (!CreatePipe(&executor->_hStdoutRead, &executor->_hStdoutWrite, NULL, 0)) {
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

  TCHAR args[] = L"args";

  BOOL ok = CreateProcess(
    L"ss_sqs.exe",
    args,
    &processSecurityAttribs,
    &threadSecurityAttribs,
    TRUE,
    0,
    NULL,
    NULL,
    &startupInfo,
    &processInfo
  );

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

  // deinitialize stdin
  CloseHandle(executor->_hStdinRead);
  CloseHandle(executor->_hStdinWrite);

  // deinitialize stdout
  CloseHandle(executor->_hStdoutRead);
  CloseHandle(executor->_hStdoutWrite);
} // appCloseExecutor function end

// app_daemon_executor.cpp file end