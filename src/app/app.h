#ifndef APP_H_
#define APP_H_

#ifndef _WIN32
#error WINAPI REQUIRED FOR THIS IMPLEMENTATION!!!
#endif

#include <windows.h>
#include <stdio.h>
#include <stdint.h>

/// @brief common start for all pipes launched by this application name
#define APP_PIPE_BASE               L"\\\\.\\pipe\\SS_SQS_"

/// @brief daemon command input pipe name
#define APP_DAEMON_COMMAND_PIPE     APP_PIPE_BASE L"DAEMON_COMMAND"

/// @brief executor output pipe name
#define APP_EXECUTOR_OUTPUT_PIPE    APP_PIPE_BASE L"EXECUTOR_OUTPUT"

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
