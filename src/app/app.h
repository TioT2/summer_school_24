#ifndef APP_H_
#define APP_H_

#ifndef _WIN32
#error WINAPI REQUIRED FOR THIS IMPLEMENTATION
#endif

#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#include "sqs/sqs.h"

/// @brief common start for all pipes launched by this application name
#define APP_PIPE_BASE               L"\\\\.\\pipe\\SS_SQS_"

/// @brief daemon<->client pipe name
#define APP_DAEMON_CLIENT_PIPE      APP_PIPE_BASE L"DAEMON_CLIENT"

/// @brief daemon<->executor pipe name
#define APP_DAEMON_EXECUTOR_PIPE    APP_PIPE_BASE L"DAEMON_EXECUTOR"

/// @brief path legnth
#define APP_PATH_LENGTH 256

typedef enum __AppDaemonRequestType {
  /// run test file, sends path relative to server
  APP_DAEMON_REQUEST_TYPE_TEST,

  /// solve single equation
  APP_DAEMON_REQUEST_TYPE_SOLVE,

  /// shutdown request
  APP_DAEMON_REQUEST_TYPE_SHUTDOWN,
} AppDaemonRequestType;

typedef struct __AppDaemonRunTestRequest {
  char testPath[APP_PATH_LENGTH]; // path of test to run
} AppDaemonRunTestRequest;

typedef struct __AppDaemonSolveRequest {
  SqsQuadraticEquationCoefficents coefficents; // coefficents for daemon to solve equation with
} AppDaemonSolveRequest;

/// @brief status of daemon solve response
typedef enum __AppDaemonSolveResponseStatus {
  /// ok
  APP_DAEMON_SOLVE_RESPONSE_STATUS_OK,

  /// error (executor error in the most cases)
  APP_DAEMON_SOLVE_RESPONSE_STATUS_ERROR,
} AppDaemonSolveResponseStatus;

/// @brief response on daemon solve request representation structure
typedef struct __AppDaemonSolveResponse {
  AppDaemonSolveResponseStatus status;   // Response status
  SqsQuadraticSolution         solution; // Actually, solution
} AppDaemonSolveResponse;

/***
 * Entry points of different project modes
 ***/


//----------------------------------------------------------------
//! @brief client entry point
//!
//! @param [in] argc count of input parameters
//! @param [in] argv parameter strings
//! 
//! @return exit status
//----------------------------------------------------------------
int
appClientMain( int argc, const char **argv );

//----------------------------------------------------------------
//! @brief daemon entry point
//!
//! @param [in] argc count of input parameters
//! @param [in] argv parameter strings
//! 
//! @return exit status
//----------------------------------------------------------------
int
appDaemonMain( int argc, const char **argv );

//----------------------------------------------------------------
//! @brief executor entry point
//!
//! @param [in] argc count of input parameters
//! @param [in] argv parameter strings
//! 
//! @return exit status
//----------------------------------------------------------------
int
appExecutorMain( int argc, const char **argv );



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

#endif
