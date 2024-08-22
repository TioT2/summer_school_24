#include "app.h"

int
appClientMain( int argc, const char **argv ) {
  HANDLE daemonCommandFile = CreateFile(
    APP_DAEMON_CLIENT_PIPE,
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    OPEN_EXISTING,
    0,
    NULL
  );

  if (daemonCommandFile == INVALID_HANDLE_VALUE) {
    printf(
      "FATAL ERORR:"
      "Can't open daemon handle\n"
    );
    return 0;
  }

  while (TRUE) {
    char buffer[256] = {0};

    printf(">>> ");

    for (int i = 0; i < sizeof(buffer) - 1; i++) {
      char c = (char)getchar();
      if (c == '\n') {
        buffer[i] = '\0';
        break;
      }
      buffer[i] = c;
    }

    const char solveCommand[] = "solve";
    const char testCommand[] = "test";
    const char shutdownCommand[] = "shutdown";

    if (strncmp(buffer, solveCommand, sizeof(solveCommand) - 1) == 0) {
      // parse solve arguments
      AppDaemonSolveRequest req = {0};

      int parsedCount = sscanf_s(
        buffer + sizeof(solveCommand),
        "%f %f %f",
        &req.coefficents.a,
        &req.coefficents.b,
        &req.coefficents.c
      );

      if (parsedCount != 3) {
        printf("Invalid quadratic coefficent string\n");
        continue;
      }

      AppDaemonRequestType requestType = APP_DAEMON_REQUEST_TYPE_SOLVE;
      WriteFile(daemonCommandFile, &requestType, sizeof(requestType), NULL, NULL);
      WriteFile(daemonCommandFile, &req, sizeof(req), NULL, NULL);

      AppDaemonSolveResponse res = {};

      if (!ReadFile(daemonCommandFile, &res, sizeof(res), NULL, NULL)) {
        continue;
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

      continue;
    }

    if (strncmp(buffer, testCommand, sizeof(testCommand) - 1) == 0) {
      AppDaemonRunTestRequest req = {0};

      int parsedCount = sscanf_s(
        buffer + sizeof(testCommand),
        "%s",
        req.testPath,
        (uint32_t)sizeof(req.testPath)
      );

      if (parsedCount != 1) {
        printf("Error during path parsing.");
        continue;
      }

      AppDaemonRequestType requestType = APP_DAEMON_REQUEST_TYPE_TEST;
      WriteFile(daemonCommandFile, &requestType, sizeof(requestType), NULL, NULL);
      WriteFile(daemonCommandFile, &req, sizeof(req), NULL, NULL);

      continue;
    }

    if (strncmp(buffer, shutdownCommand, sizeof(shutdownCommand) - 1) == 0) {
      AppDaemonRequestType requestType = APP_DAEMON_REQUEST_TYPE_SHUTDOWN;
      WriteFile(daemonCommandFile, &requestType, sizeof(requestType), NULL, NULL);
      break;
    }
  }

  CloseHandle(daemonCommandFile);

  return 0;
} // appClientMain function end

// app_client.cpp file end