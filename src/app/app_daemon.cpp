#include "app.h"

int
appDaemonMain( int argc, const char **argv ) {
  printf("DAEMON STARTED\n");

  HANDLE commandPipe = CreateNamedPipe(
    APP_DAEMON_CLIENT_PIPE,
    PIPE_ACCESS_DUPLEX,
    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
    PIPE_UNLIMITED_INSTANCES,
    512,
    512,
    5000,
    NULL
  );

  if (commandPipe == INVALID_HANDLE_VALUE) {
    printf("Failed to open input daemon pipe. ");
    appPrintWinapiError(stdout, GetLastError());
    printf("\n");
    return 1;
  }

  while (TRUE) {
    printf("Waiting client... ");
    int connected = ConnectNamedPipe(commandPipe, NULL);

    if (connected) {
      printf("Connected!\n");
    } else {
      printf("Error connecting to command pipe\n");
      continue;
    }

    BOOL continueSession = TRUE;

    while (continueSession) {
      AppDaemonRequestType requestType = APP_DAEMON_REQUEST_TYPE_SHUTDOWN;

      if (!ReadFile(commandPipe, &requestType, sizeof(requestType), NULL, NULL)) {
        break;
      }

      switch (requestType) {
      case APP_DAEMON_REQUEST_TYPE_TEST : {
        AppDaemonRunTestRequest req = {0};

        if (ReadFile(commandPipe, &req, sizeof(req), NULL, NULL)) {
          printf("  TEST %s\n", req.testPath);
        } else {
          printf("  TEST <invalid>\n");
        }

        break;
      }

      case APP_DAEMON_REQUEST_TYPE_SOLVE    : {
        AppDaemonSolveRequest req = {0};

        if (ReadFile(commandPipe, &req, sizeof(req), NULL, NULL)) {
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
        WriteFile(commandPipe, &res, sizeof(res), NULL, NULL);

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
    DisconnectNamedPipe(commandPipe);
  }

  CloseHandle(commandPipe);

  return 0;
} // appDaemonMain function end

// app_daemon.cpp file end
