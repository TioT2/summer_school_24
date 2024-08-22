#include "app.h"

/// @brief executor representation structure
typedef struct __AppExecutor {
  HANDLE _hStdinRead;   // STDIN  pipe read
  HANDLE _hStdinWrite;  // STDIN  pipe write
  HANDLE _hStdoutRead;  // STDOUT pipe read
  HANDLE _hStdoutWrite; // STDOUT pipe write
  wchar_t *_cmdLine;    // command line

  HANDLE hStdin;        // STDIN  handle for user
  HANDLE hStdout;       // STDOUT handle for user
} AppExecutor;


static wchar_t *
appConnectArguments( int argc, const char **argv ) {
  size_t cmdLineLen = 1;

  for (int i = 0; i < argc; i++) {
    cmdLineLen += strlen(argv[i]);
    cmdLineLen++;
  }

  wchar_t *cmdLine = (wchar_t *)malloc(sizeof(wchar_t) * cmdLineLen);

  if (cmdLine == NULL)
    return NULL;

  wchar_t *cmdLineIter = cmdLine;

  for (int argI = 0; argI < argc; argI++) {
    const char *const arg = argv[argI];
    const size_t len = strlen(arg);

    for (size_t charI = 0; charI < len; charI++)
      *cmdLineIter++ = (wchar_t)arg[charI];
    *cmdLineIter++ = L' ';
  }

  return cmdLine;
}

static BOOL
appOpenExecutor( int argc, const char **argv, AppExecutor *const executor ) {
  assert(executor != NULL);

  if ((executor->_cmdLine = appConnectArguments(argc, argv)) == NULL)
    return FALSE;

  if (!CreatePipe(&executor->_hStdinRead,  &executor->_hStdinWrite,  NULL, 0))
    return FALSE;

  if (!CreatePipe(&executor->_hStdoutRead, &executor->_hStdoutWrite, NULL, 0))
    return FALSE;

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

  return CreateProcess(
    L"ss_sqs.exe",
    executor->_cmdLine,
    &processSecurityAttribs,
    &threadSecurityAttribs,
    TRUE,
    0,
    NULL,
    NULL,
    &startupInfo,
    &processInfo
  );
} // appOpenExecutor function end

static void
appCloseExecutor( AppExecutor *const executor ) {
  assert(executor != NULL);

  // deinitialize stdin
  CloseHandle(executor->_hStdinRead);
  CloseHandle(executor->_hStdinWrite);

  // deinitialize stdout
  CloseHandle(executor->_hStdoutRead);
  CloseHandle(executor->_hStdoutWrite);

  // free command line arguments
  free(executor->_cmdLine);
} // appCloseExecutor function end

int
appDaemonMain( int argc, const char **argv ) {
  printf("DAEMON STARTED\n");

  HANDLE clientPipe = CreateNamedPipe(
    APP_DAEMON_CLIENT_PIPE,
    PIPE_ACCESS_DUPLEX,
    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
    PIPE_UNLIMITED_INSTANCES,
    512,
    512,
    5000,
    NULL
  );

  if (clientPipe == INVALID_HANDLE_VALUE) {
    printf("Failed to open input daemon pipe. ");
    appPrintWinapiError(stdout, GetLastError());
    printf("\n");
    return 1;
  }

  while (TRUE) {
    printf("Waiting client... ");
    int connected = ConnectNamedPipe(clientPipe, NULL);

    if (connected) {
      printf("Connected!\n");
    } else {
      printf("Error connecting to command pipe\n");
      continue;
    }

    BOOL continueSession = TRUE;

    while (continueSession) {
      AppDaemonRequestType requestType = APP_DAEMON_REQUEST_TYPE_SHUTDOWN;

      if (!ReadFile(clientPipe, &requestType, sizeof(requestType), NULL, NULL)) {
        break;
      }

      switch (requestType) {

      case APP_DAEMON_REQUEST_TYPE_TEST : {
        AppDaemonTestRequest req = {0};

        if (ReadFile(clientPipe, &req, sizeof(req), NULL, NULL)) {
          printf("  TEST %s\n", req.testPath);
        } else {
          printf("  TEST <invalid>\n");
        }

        AppDaemonTestResponseHeader header = {
          .status = APP_DAEMON_TEST_RESPONSE_STATUS_TEST_DOESNT_EXIST,
        };

        WriteFile(clientPipe, &header, sizeof(header), NULL, NULL);

        break;
      }

      case APP_DAEMON_REQUEST_TYPE_SOLVE    : {
        AppDaemonSolveRequest req = {0};

        if (ReadFile(clientPipe, &req, sizeof(req), NULL, NULL)) {
          printf(
            "  SOLVE (%f %f %f)\n",
            req.coefficents.a,
            req.coefficents.b,
            req.coefficents.c
          );
        } else {
          printf("  SOLVE <invalid>\n");
        }

        AppDaemonSolveResponse res = {
          .status = APP_DAEMON_SOLVE_RESPONSE_STATUS_ERROR,
        };

        // Send response
        WriteFile(clientPipe, &res, sizeof(res), NULL, NULL);

        break;
      }

      case APP_DAEMON_REQUEST_TYPE_SHUTDOWN : {
        printf("  SHUTDOWN\n");
        continueSession = FALSE;
        break;
      }

      default: {
        printf("  INVALID REQUEST TYPE: %d\n", requestType);
        continueSession = FALSE;
        break;
      }
      }
    }

    printf("Client disconnected.\n");
    DisconnectNamedPipe(clientPipe);
  }

  CloseHandle(clientPipe);

  return 0;
} // appDaemonMain function end

// app_daemon.cpp file end
