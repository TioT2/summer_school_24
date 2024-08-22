#ifndef APP_DAEMON_EXECUTOR_H_
#define APP_DAEMON_EXECUTOR_H_

#include <windows.h>

/// @brief executor representation structure
typedef struct __AppExecutor {
  HANDLE _hStdinRead;   // STDIN  pipe read
  HANDLE _hStdinWrite;  // STDIN  pipe write
  HANDLE _hStdoutRead;  // STDOUT pipe read
  HANDLE _hStdoutWrite; // STDOUT pipe write

  HANDLE hStdin;        // STDIN  handle for user
  HANDLE hStdout;       // STDOUT handle for user
} AppExecutor;

typedef enum __AppExecutorTaskType {
  /// solve quadratic and write answer
  APP_EXECUTOR_TASK_TYPE_SOLVE,

  /// execute test
  APP_EXECUTOR_TASK_TYPE_TEST,

  /// terminate
  APP_EXECUTOR_TASK_TYPE_QUIT,
} AppExecutorTaskType;

typedef enum __AppExecutorTaskStatus {
  APP_EXECUTOR_TASK_STATUS_OK,      // ok
  APP_EXECUTOR_TASK_STATUS_CRASHED, // crashed
} AppExecutorTaskStatus;

typedef struct __AppExecutorCrashReport {
  int signal; /// signal application crashed with
} AppExecutorCrashReport;

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

#endif // !defined(APP_DAEMON_EXECUTOR_H_)

// app_daemon_executor.h file end#pragma once
