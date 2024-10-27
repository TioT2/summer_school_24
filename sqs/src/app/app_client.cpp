#include "app.h"

/// @brief client execution context representation structure
typedef struct __AppClientContext {
  HANDLE daemonPipe; ///< daemon pipe handle
} AppClientContext;

//----------------------------------------------------------------
//! @brief 'solve' command handling function
//!
//! @param [in] context client context
//! @param [in] buffer  command parameter buffer
//----------------------------------------------------------------
static void
appClientHandleSolve( AppClientContext *const context, const char *buffer ) {
  // parse solve arguments
  AppDaemonSolveRequest req = {0};

  int parsedCount = sscanf_s(
    buffer,
    "%f %f %f",
    &req.coefficents.a,
    &req.coefficents.b,
    &req.coefficents.c
  );

  if (parsedCount != 3) {
    printf("Invalid quadratic coefficent string\n");
    return;
  }

  AppDaemonRequestType requestType = APP_DAEMON_REQUEST_TYPE_SOLVE;
  WriteFile(context->daemonPipe, &requestType, sizeof(requestType), NULL, NULL);
  WriteFile(context->daemonPipe, &req, sizeof(req), NULL, NULL);

  AppDaemonSolveResponse res = {};

  if (!ReadFile(context->daemonPipe, &res, sizeof(res), NULL, NULL)) {
    return;
  }

  printf("--> ");

  switch (res.status) {
  case APP_DAEMON_SOLVE_RESPONSE_STATUS_OK:
    printf("OK: ");
    sqsPrintQuadraticSolution(stdout, &res.solution);
    break;

  case APP_DAEMON_SOLVE_RESPONSE_STATUS_ERROR:
    printf("ERROR");
    break;
  }

  printf("\n");
} // appClientHandleSolve function end

//----------------------------------------------------------------
//! @brief 'test' command handling function
//!
//! @param [in] context client context
//! @param [in] buffer  command parameter buffer
//----------------------------------------------------------------
static void
appClientHandleTest( AppClientContext *const context, const char *buffer ) {
  AppDaemonTestRequest req = {0};

  int parsedCount = sscanf_s(
    buffer,
    "%s",
    req.testPath,
    (uint32_t)sizeof(req.testPath)
  );

  if (parsedCount != 1) {
    printf("Error during path parsing.");
    return;
  }

  AppDaemonRequestType requestType = APP_DAEMON_REQUEST_TYPE_TEST;
  WriteFile(context->daemonPipe, &requestType, sizeof(requestType), NULL, NULL);
  WriteFile(context->daemonPipe, &req, sizeof(req), NULL, NULL);

  AppDaemonTestResponseHeader header = {
    .status = APP_DAEMON_TEST_RESPONSE_STATUS_TEST_DOESNT_EXIST,
  };

  if (!ReadFile(context->daemonPipe, &header, sizeof(header), NULL, NULL)) {
    printf("Error reading server response head\n");
    return;
  }

  printf("--> ");
  switch (header.status) {
  case APP_DAEMON_TEST_RESPONSE_STATUS_OK                 : {
    printf("OK\n");

    AppDaemonTestResponseEntry entry = {
      .executorStatus = APP_DAEMON_TEST_RESPONSE_EXECUTOR_STATUS_NORMALLY_EXECUTED,
    };

    for (uint32_t i = 0; i < header.entryCount; i++) {
      if (!ReadFile(context->daemonPipe, &entry, sizeof(entry), NULL, NULL)) {
        printf("    ERROR READING DAEMON ENTRY\n");
        break;
      }

      switch (entry.executorStatus) {
      case APP_DAEMON_TEST_RESPONSE_EXECUTOR_STATUS_NORMALLY_EXECUTED: {
        printf(
          CLI_SET_FOREGROUND_COLOR(0, 255, 0)
          "    OK      "
          CLI_RESET_COLOR()
        );
        sqsPrintTestQuadraticFeedback(stdout, i + 1, &entry.feedback);
        printf("\n");
        break;
      }

      case APP_DAEMON_TEST_RESPONSE_EXECUTOR_STATUS_EXECUTOR_CRASHED: {
        printf(
          CLI_SET_FOREGROUND_COLOR(255, 0, 0)
          "    CRASHED\n"
          CLI_RESET_COLOR()
        );
        break;
      }
      }
    }
    break;
  }

  case APP_DAEMON_TEST_RESPONSE_STATUS_TEST_DOESNT_EXIST  : {
    printf("TEST FILE DOESN'T EXIST\n");
    break;
  }

  case APP_DAEMON_TEST_RESPONSE_STATUS_TEST_PARSING_ERROR : {
    printf("TEST PARSING ERROR\n");
    break;
  }

  case APP_DAEMON_TEST_RESPONSE_STATUS_EXECUTOR_CRASHED: {
    printf("EXECUTOR CRASHED\n");
    break;
  }
  }
} // appClientHandleTest function end

//----------------------------------------------------------------
//! @brief 'quit' command handling function
//!
//! @param [in] context client context
//! @param [in] buffer  command parameter buffer
//----------------------------------------------------------------
static void
appClientHandleQuit( AppClientContext *const context, const char * ) {
  AppDaemonRequestType requestType = APP_DAEMON_REQUEST_TYPE_QUIT;
  WriteFile(context->daemonPipe, &requestType, sizeof(requestType), NULL, NULL);
} // appClientHandleQuit function end

//----------------------------------------------------------------
//! @brief 'help' command handling function
//!
//! @param [in] context client context
//! @param [in] buffer  command parameter buffer
//----------------------------------------------------------------
static void
appClientHandleHelp( AppClientContext *const, const char * ) {
  printf(
    "COMMANDS\n"
    "  Show commands                      help\n"
    "  Solve quadratic equation           solve [float] [float] [float]\n"
    "  Run test file                      test [path_to_test]\n"
    "  Quit from program                  quit\n"
  );
} // appClientHandleHelp function end

//----------------------------------------------------------------
//! @brief client startup function
//!
//! @param [in] context client context to run client in
//----------------------------------------------------------------
static void
appRunClient( AppClientContext *const context ) {
  context->daemonPipe = CreateFile(
    APP_DAEMON_CLIENT_PIPE,
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    OPEN_EXISTING,
    0,
    NULL
  );

  if (context->daemonPipe == INVALID_HANDLE_VALUE) {
    printf(
      CLI_SET_FOREGROUND_COLOR(255, 0, 0)
      "FATAL ERORR:"
      CLI_RESET_COLOR()
      "Can't open daemon handle\n"
    );
    return;
  }

  while (TRUE) {
    printf(">>> ");

    char buffer[256] = {0};

    // read string from input
    cliGetString(buffer, sizeof(buffer));

    const static struct __AppCommand {
      const char *command;                                /// command name pointer
      void (*handle)( AppClientContext *, const char * ); /// handle pointer
      BOOL doReturn;                                      /// TRUE if command should end execution
    } commands[] = {
      {"solve", appClientHandleSolve, FALSE},
      {"test" , appClientHandleTest , FALSE},
      {"quit" , appClientHandleQuit , TRUE },
      {"help" , appClientHandleHelp , FALSE},
    };

    BOOL doBreak = FALSE;

    for (uint32_t i = 0, n = sizeof(commands) / sizeof(commands[0]); i < n; i++) {
      const struct __AppCommand *const command = commands + i;
      size_t len = strlen(command->command);

      if (strncmp(buffer, command->command, len) == 0) {
        doBreak |= command->doReturn;
        command->handle(context, buffer + len + 1);
        break;
      }
    }

    if (doBreak)
      break;
  }

  CloseHandle(context->daemonPipe);
} // appRunClient function end

int
appClientMain( int, const char ** ) {
  AppClientContext context = {0};

  appRunClient(&context);

  return 0;
} // appClientMain function end

// app_client.cpp file end
