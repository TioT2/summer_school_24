#ifndef APP_EXECUTOR_INTERFACE_H_
#define APP_EXECUTOR_INTERFACE_H_

#include <windows.h>
#include <stdio.h>

/// @brief executor representation structure
typedef struct __AppExecutor {
  HANDLE _hStdinRead;   ///< STDIN  pipe read
  HANDLE _hStdinWrite;  ///< STDIN  pipe write
  HANDLE _hStdoutRead;  ///< STDOUT pipe read
  HANDLE _hStdoutWrite; ///< STDOUT pipe write

  HANDLE _hProcess;     ///< process handle
  HANDLE _hThread;      ///< thread handle

  HANDLE hStdin;        ///< public STDIN  handle
  HANDLE hStdout;       ///< public STDOUT handle
} AppExecutor;

/// @brief executor task type
typedef enum __AppExecutorTaskType {
  /// solve quadratic and write answer
  APP_EXECUTOR_TASK_TYPE_SOLVE,

  /// execute test
  APP_EXECUTOR_TASK_TYPE_TEST,

  /// terminate
  APP_EXECUTOR_TASK_TYPE_QUIT,
} AppExecutorTaskType;

/// @brief executor task
typedef enum __AppExecutorTaskStatus {
  /// ok
  APP_EXECUTOR_TASK_STATUS_OK,

  /// crashed
  APP_EXECUTOR_TASK_STATUS_CRASHED,
} AppExecutorTaskStatus;

// @brief executor crash report representation structure
typedef struct __AppExecutorCrashReport {
  int signal;          ///< signal application crashed with
  int traceFrameCount; ///< stacktrace depth (<= 32)
} AppExecutorCrashReport;

/// @brief maximal stack trace name len
#define APP_EXECUTOR_STACK_TRACE_NAME_LEN     (256)

/// @brief unknown line index
#define APP_EXECUTOR_STACK_TRACE_LINE_UNKNOWN (~0U)

/// @brief stackframe representation structure
typedef struct __AppExecutorStackFrame {
  char moduleName[APP_EXECUTOR_STACK_TRACE_NAME_LEN]; ///< module name
  char symbolName[APP_EXECUTOR_STACK_TRACE_NAME_LEN]; ///< function name
  int  line;                                 ///< line
} AppExecutorStackFrame;

//----------------------------------------------------------------
//! @brief executor initialization function
//!
//! @param [out] executor executor to initialize pointer
//! 
//! @return TRUE if success, FALSE otherwise
//----------------------------------------------------------------
BOOL
appOpenExecutor( AppExecutor *const executor );

//----------------------------------------------------------------
//! @brief executor deinitialization function
//!
//! @param [out] executor executor to deinitialize pointer
//----------------------------------------------------------------
void
appCloseExecutor( AppExecutor *const executor );


/***
 * Utility functions
 ***/

//----------------------------------------------------------------
//! @brief WINAPI error display function
//!
//! @param [in] file  file to print winapi error to
//! @param [in] error WINAPI error code
//! 
//! @return cuont of characters displayed
//----------------------------------------------------------------
int
appPrintWinapiError( FILE *const file, DWORD error );

#endif // !defined(APP_EXECUTOR_INTERFACE_H_)

// app_executor_interface.h file end#pragma once
