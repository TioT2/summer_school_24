#include "app.h"

int
appClientMain( int argc, const char **argv ) {
  HANDLE daemonPipe = CreateFile(
    APP_DAEMON_CLIENT_PIPE,
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    OPEN_EXISTING,
    0,
    NULL
  );

  if (daemonPipe == INVALID_HANDLE_VALUE) {
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
      WriteFile(daemonPipe, &requestType, sizeof(requestType), NULL, NULL);
      WriteFile(daemonPipe, &req, sizeof(req), NULL, NULL);

      AppDaemonSolveResponse res = {};

      if (!ReadFile(daemonPipe, &res, sizeof(res), NULL, NULL)) {
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
      AppDaemonTestRequest req = {0};

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
      WriteFile(daemonPipe, &requestType, sizeof(requestType), NULL, NULL);
      WriteFile(daemonPipe, &req, sizeof(req), NULL, NULL);

      AppDaemonTestResponseHeader header = {
        .status = APP_DAEMON_TEST_RESPONSE_STATUS_TEST_DOESNT_EXIST,
      };

      if (!ReadFile(daemonPipe, &header, sizeof(header), NULL, NULL)) {
        printf("Error reading server response head\n");
        continue;
      }

      printf("--> ");
      switch (header.status) {
      case APP_DAEMON_TEST_RESPONSE_STATUS_OK                 : {
        printf("OK\n");

        AppDaemonTestResponseEntry entry = {
          .executorStatus = APP_DAEMON_TEST_RESPONSE_EXECUTOR_STATUS_NORMALLY_EXECUTED,
        };

        for (uint32_t i = 0; i < header.entryCount; i++) {
          if (!ReadFile(daemonPipe, &entry, sizeof(entry), NULL, NULL)) {
            printf("    ERROR READING DAEMON ENTRY\n");
            break;
          }

          switch (entry.executorStatus) {
          case APP_DAEMON_TEST_RESPONSE_EXECUTOR_STATUS_NORMALLY_EXECUTED: {
            printf("    OK      ");
            sqsPrintTestQuadraticFeedback(stdout, i + 1, &entry.feedback);
            printf("\n");
            break;
          }

          case APP_DAEMON_TEST_RESPONSE_EXECUTOR_STATUS_EXECUTOR_CRASHED: {
            printf("    CRASHED\n");
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

      continue;
    }

    if (strncmp(buffer, shutdownCommand, sizeof(shutdownCommand) - 1) == 0) {
      AppDaemonRequestType requestType = APP_DAEMON_REQUEST_TYPE_SHUTDOWN;
      WriteFile(daemonPipe, &requestType, sizeof(requestType), NULL, NULL);
      break;
    }
  }

  CloseHandle(daemonPipe);

  return 0;
} // appClientMain function end

// app_client.cpp file end