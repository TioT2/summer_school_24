#ifndef APP_H_
#define APP_H_

#ifndef _WIN32
#error WINAPI REQUIRED FOR THIS IMPLEMENTATION
#endif

#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#include "sqs/sqs.h"
#include "sqs/test/sqs_test.h"
#include "executor_interface/app_executor_interface.h"

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

/***
 * Test request/responce handling
 ***/

/// @brief test file run request representation structure
typedef struct __AppDaemonTestRequest {
  char testPath[APP_PATH_LENGTH]; /// path of test to run
} AppDaemonTestRequest;

/// @brief response status representation structure
typedef enum __AppDaemonTestResponseStatus {
  /// ok
  APP_DAEMON_TEST_RESPONSE_STATUS_OK,

  /// executor crashed before test execution
  APP_DAEMON_TEST_RESPONSE_STATUS_EXECUTOR_CRASHED,

  /// test file doesn't exist
  APP_DAEMON_TEST_RESPONSE_STATUS_TEST_DOESNT_EXIST,

  /// test file parsing error
  APP_DAEMON_TEST_RESPONSE_STATUS_TEST_PARSING_ERROR,
} AppDaemonTestResponseStatus;

/// @brief test response header representation structure
typedef struct __AppDaemonTestResponseHeader {
  AppDaemonTestResponseStatus status;     /// response status
  uint32_t                    entryCount; /// count of test entries
} AppDaemonTestResponseHeader;

/// @brief executor status during test execution representation structure
typedef enum __AppDaemonTestResponseExecutorStatus {
  /// executor application normally worked
  APP_DAEMON_TEST_RESPONSE_EXECUTOR_STATUS_NORMALLY_EXECUTED,

  /// executor application crashed during execution
  APP_DAEMON_TEST_RESPONSE_EXECUTOR_STATUS_EXECUTOR_CRASHED,
} AppDaemonTestResponseExecutorStatus;

/// @brief test run request response element representation structure
typedef struct __AppDaemonTestResponseEntry {
  AppDaemonTestResponseExecutorStatus executorStatus; /// executor status
  SqsTestQuadraticFeedback            feedback;       /// executor feedback
} AppDaemonTestResponseEntry;


/***
 * Solve request/responce handling
 ***/

/// @brief solve request representation structure
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
