#ifndef APP_EXECUTOR_INTERFACE_H_
#define APP_EXECUTOR_INTERFACE_H_

#include <windows.h>
#include <stdio.h>

/// @brief executor representation structure
typedef struct __AppExecutor {
  HANDLE _hStdinRead;   // STDIN  pipe read
  HANDLE _hStdinWrite;  // STDIN  pipe write
  HANDLE _hStdoutRead;  // STDOUT pipe read
  HANDLE _hStdoutWrite; // STDOUT pipe write

  HANDLE _hProcess;     // process handle
  HANDLE _hThread;      // thread handle

  HANDLE hStdin;        // public STDIN  handle
  HANDLE hStdout;       // public STDOUT handle
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
  APP_EXECUTOR_TASK_STATUS_OK,      // ok
  APP_EXECUTOR_TASK_STATUS_CRASHED, // crashed
} AppExecutorTaskStatus;

// @brief executor crash report representation structure
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
