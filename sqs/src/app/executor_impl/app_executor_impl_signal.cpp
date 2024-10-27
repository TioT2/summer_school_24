
#include <windows.h>
#include <winnt.h>
#include <DbgHelp.h>

#include "app_executor_impl.h"

#pragma comment(lib, "dbghelp.lib")

/// @brief STDOUT global constant
static HANDLE appExecutorStdout = NULL;

/// @brief STDIN global constant
static HANDLE appExecutorStdin = NULL;

//----------------------------------------------------------------
//! @brief signal handling function
//!
//! @param [in] signal signal index
//! 
//! @note writes crash report to stdout
//----------------------------------------------------------------
static void
appSignalHandler( int signal ) {
  VOID *tracePointers[48] = {0};
  DWORD traceHash[48] = {0};

  HANDLE hProcess = GetCurrentProcess();

  WORD traceFrameCount = RtlCaptureStackBackTrace(0, 32, tracePointers, traceHash);
  
  if (!SymInitialize(hProcess, NULL, TRUE)) {
    // stop stacktrace reading
    traceFrameCount = 0;
  }

  AppExecutorTaskStatus taskStatus = APP_EXECUTOR_TASK_STATUS_CRASHED;
  AppExecutorCrashReport crashReport = {
    .signal = signal,
    .traceFrameCount = traceFrameCount,
  };

  // write header
  WriteFile(appExecutorStdout, &taskStatus, sizeof(taskStatus), NULL, NULL);
  WriteFile(appExecutorStdout, &crashReport, sizeof(crashReport), NULL, NULL);

  AppExecutorStackFrame stackFrame = {0};

  uint8_t symbolInfoLocation[sizeof(SYMBOL_INFO) + APP_EXECUTOR_STACK_TRACE_NAME_LEN] = {0};
  SYMBOL_INFO *symbolInfo = reinterpret_cast<SYMBOL_INFO *>(symbolInfoLocation);
  IMAGEHLP_LINE64 line = { .SizeOfStruct = sizeof(line) };

  for (WORD i = 0; i < traceFrameCount; i++) {
    DWORD64 traceAddr = (DWORD64)tracePointers[i];
    HMODULE hTraceModule = (HMODULE)SymGetModuleBase64(hProcess, traceAddr);

    memset(symbolInfo, 0, sizeof(symbolInfoLocation));

    symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbolInfo->MaxNameLen = APP_EXECUTOR_STACK_TRACE_NAME_LEN - 1;

    const char *const unknown = "<unknown>";

    // get symbol name
    DWORD64 symDisp = 0;
    if (SymFromAddr(hProcess, traceAddr, &symDisp, symbolInfo)) {
      memcpy(stackFrame.symbolName, symbolInfo->Name, APP_EXECUTOR_STACK_TRACE_NAME_LEN);
    } else {
      memcpy(stackFrame.symbolName, unknown, strlen(unknown));
    }

    // get module name
    if (!GetModuleFileNameA(hTraceModule, stackFrame.moduleName, APP_EXECUTOR_STACK_TRACE_NAME_LEN - 1)) {
      memcpy(stackFrame.moduleName, unknown, strlen(unknown));
    }

    // get line
    DWORD lineDisp = 0;
    memset(&line, 0, sizeof(line));
    line.SizeOfStruct = sizeof(line);
    if (!SymGetLineFromAddr64(hProcess, traceAddr, &lineDisp, &line)) {
      stackFrame.line = APP_EXECUTOR_STACK_TRACE_LINE_UNKNOWN;
    }

    // send stackFrame to daemon
    WriteFile(appExecutorStdout, &stackFrame, sizeof(stackFrame), NULL, NULL);
  }
} // appSignalHandler function end

void
appExecutorSetupSignals( HANDLE hStdin, HANDLE hStdout ) {
  appExecutorStdout = hStdout;
  appExecutorStdin = hStdin;

  // setup signal handlers
  signal(SIGINT  , appSignalHandler);
  signal(SIGILL  , appSignalHandler);
  signal(SIGFPE  , appSignalHandler);
  signal(SIGSEGV , appSignalHandler);
  signal(SIGTERM , appSignalHandler);
  signal(SIGBREAK, appSignalHandler);
  signal(SIGABRT , appSignalHandler);
} // appExecutorSetupSignals function end

// app_executor_impl_signal.cpp function endw