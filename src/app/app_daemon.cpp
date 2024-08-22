#include "app.h"
#include "executor_interface/app_executor_interface.h"

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

        header.status = APP_DAEMON_TEST_RESPONSE_STATUS_TEST_DOESNT_EXIST;
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

        AppExecutor executor = {0};

        // try open executor
        if (appOpenExecutor(&executor)) {
          AppExecutorTaskType taskType;
          
          assert(executor.hStdin != NULL);
          assert(executor.hStdout != NULL);

          taskType = APP_EXECUTOR_TASK_TYPE_SOLVE;
          WriteFile(executor.hStdin, &taskType, sizeof(taskType), NULL, NULL);
          
          WriteFile(executor.hStdin, &req.coefficents, sizeof(req.coefficents), NULL, NULL);

          AppExecutorTaskStatus taskStatus = (AppExecutorTaskStatus)-1;
          ReadFile(executor.hStdout, &taskStatus, sizeof(taskStatus), NULL, NULL);

          if (taskStatus != APP_EXECUTOR_TASK_STATUS_CRASHED) {
            // read solution
            res.status = APP_DAEMON_SOLVE_RESPONSE_STATUS_OK;
            ReadFile(executor.hStdout, &res.solution, sizeof(res.solution), NULL, NULL);

            // send quit task
            taskType = APP_EXECUTOR_TASK_TYPE_QUIT;
            WriteFile(executor.hStdin, &taskType, sizeof(taskType), NULL, NULL);
          } else {
            res.status = APP_DAEMON_SOLVE_RESPONSE_STATUS_ERROR;

            // read crash report
            AppExecutorCrashReport crashReport = {0};
            ReadFile(executor.hStdout, &crashReport, sizeof(crashReport), NULL, NULL);
            printf("  EXECUTOR CRASHED WITH %d SIGNAL\n", crashReport.signal);
          }

        }

        appCloseExecutor(&executor);

        // send response to client
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

// static wchar_t *
// appConnectArguments( int argc, const char **argv ) {
//   size_t cmdLineLen = 1;
// 
//   for (int i = 0; i < argc; i++) {
//     cmdLineLen += strlen(argv[i]);
//     cmdLineLen++;
//   }
// 
//   wchar_t *cmdLine = (wchar_t *)malloc(sizeof(wchar_t) * cmdLineLen);
// 
//   if (cmdLine == NULL)
//     return NULL;
// 
//   wchar_t *cmdLineIter = cmdLine;
// 
//   for (int argI = 0; argI < argc; argI++) {
//     const char *const arg = argv[argI];
//     const size_t len = strlen(arg);
// 
//     for (size_t charI = 0; charI < len; charI++)
//       *cmdLineIter++ = (wchar_t)arg[charI];
//     *cmdLineIter++ = L' ';
//   }
// 
//   return cmdLine;
// }

// app_daemon.cpp file end
