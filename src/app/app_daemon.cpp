#include <signal.h>

#include "app.h"
#include "executor_interface/app_executor_interface.h"

/// @brief daemon context representation structure
typedef struct __AppDaemonContext {
  HANDLE clientPipe;      /// daemon<->client pipe
  BOOL   continueSession; /// continue client session while true
} AppDaemonContext;

//----------------------------------------------------------------
//! @brief executor crash logging function
//!
//! @param [in] file        file to log report to
//! @param [in] crashReport crash report
//! @param [in] executor    executor to read report from
//----------------------------------------------------------------
static void
appDaemonLogCrashReport(
  FILE *const file,
  const AppExecutorCrashReport *const crashReport,
  const AppExecutor *const executor
) {
  AppExecutorStackFrame stackFrame;

  const char *signalName;
  switch (crashReport->signal) {
  case SIGINT   : signalName = "SIGINT";   break;
  case SIGILL   : signalName = "SIGILL";   break;
  case SIGFPE   : signalName = "SIGFPE";   break;
  case SIGSEGV  : signalName = "SIGSEGV";  break;
  case SIGTERM  : signalName = "SIGTERM";  break;
  case SIGBREAK : signalName = "SIGBREAK"; break;
  case SIGABRT  : signalName = "SIGABRT";  break;
  default       : signalName = "<unknown>";
  }

  fprintf(file, "  Executor crashed with %s signal. Stacktrace: \n", signalName);
  for (int32_t i = 0; i < crashReport->traceFrameCount; i++) {
    if (!ReadFile(executor->hStdout, &stackFrame, sizeof(stackFrame), NULL, NULL))
      return;

    fprintf(file, "    #%d:\n", i);
    fprintf(file, "      file    : %s\n", stackFrame.moduleName);
    fprintf(file, "      function: %s\n", stackFrame.symbolName);

    if (stackFrame.line == APP_EXECUTOR_STACK_TRACE_LINE_UNKNOWN)
      fprintf(file, "      line    : <unknown>\n");
    else
      fprintf(file, "      line    : %d\n", stackFrame.line);
  }
} // appDaemonLogStackFrame function end

//----------------------------------------------------------------
//! @brief TEST task handling function
//!
//! @param [in] context daemon context
//----------------------------------------------------------------
static void
appDaemonContextHandleTest( AppDaemonContext *const context ) {
  AppDaemonTestRequest req = {0};

  if (ReadFile(context->clientPipe, &req, sizeof(req), NULL, NULL)) {
    printf("  TEST %s\n", req.testPath);
  } else {
    printf("  TEST <invalid>\n");
    return;
  }

  AppDaemonTestResponseHeader header = {
    .status = APP_DAEMON_TEST_RESPONSE_STATUS_TEST_DOESNT_EXIST,
  };

  FILE *f = NULL;

  fopen_s(&f, req.testPath, "r");

  if (f == NULL) {
    header.status = APP_DAEMON_TEST_RESPONSE_STATUS_TEST_DOESNT_EXIST;
    WriteFile(context->clientPipe, &header, sizeof(header), NULL, NULL);
    return;
  }

  fseek(f, 0, SEEK_END);
  size_t fileSize = ftell(f);
  rewind(f);

  char *text = (char *)malloc(fileSize);

  if (text == NULL) {
    header.status = APP_DAEMON_TEST_RESPONSE_STATUS_TEST_PARSING_ERROR;
    WriteFile(context->clientPipe, &header, sizeof(header), NULL, NULL);
    fclose(f);
    return;
  }

  fread(text, 1, fileSize, f);
  fclose(f);

  SqsQuadraticTestSet testSet = {
    .testCount = 0,
  };

  if (!sqsParseQuadraticTestSet(text, &testSet)) {
    header.status = APP_DAEMON_TEST_RESPONSE_STATUS_TEST_PARSING_ERROR;
    WriteFile(context->clientPipe, &header, sizeof(header), NULL, NULL);
    return;
  }
  free(text);

  // initialize executor
  AppExecutor executor = {0};

  if (!appOpenExecutor(&executor)) {
    header.status = APP_DAEMON_TEST_RESPONSE_STATUS_EXECUTOR_CRASHED;
    WriteFile(context->clientPipe, &header, sizeof(header), NULL, NULL);
    return;
  }

  header.status = APP_DAEMON_TEST_RESPONSE_STATUS_OK;
  header.entryCount = testSet.testCount;
  WriteFile(context->clientPipe, &header, sizeof(header), NULL, NULL);

  uint32_t ti;
  BOOL executorOpen = TRUE;

  AppDaemonTestResponseEntry entry = {
    .executorStatus = APP_DAEMON_TEST_RESPONSE_EXECUTOR_STATUS_EXECUTOR_CRASHED,
  };

  for (ti = 0; ti < testSet.testCount; ti++) {
    const SqsQuadraticTest *test = testSet.tests + ti;

    AppExecutorTaskType taskType = APP_EXECUTOR_TASK_TYPE_TEST;

    WriteFile(executor.hStdin, &taskType, sizeof(taskType), NULL, NULL);
    WriteFile(executor.hStdin, test, sizeof(SqsQuadraticTest), NULL, NULL);

    AppExecutorTaskStatus taskStatus = (AppExecutorTaskStatus)-1;

    BOOL crashed = FALSE;

    if (!ReadFile(executor.hStdout, &taskStatus, sizeof(taskStatus), NULL, NULL)) {
      crashed = TRUE;
    } else {
      crashed = (taskStatus == APP_EXECUTOR_TASK_STATUS_CRASHED);

      if (taskStatus == APP_EXECUTOR_TASK_STATUS_CRASHED) {
        AppExecutorCrashReport crashReport = {0};

        if (ReadFile(executor.hStdout, &crashReport, sizeof(crashReport), NULL, NULL))
          appDaemonLogCrashReport(stdout, &crashReport, &executor);
      }

    }

    if (!crashed) {
      if (!ReadFile(executor.hStdout, &entry.feedback, sizeof(entry.feedback), NULL, NULL)) {
        crashed = TRUE;
      }
    }

    if (crashed) {
      entry.executorStatus = APP_DAEMON_TEST_RESPONSE_EXECUTOR_STATUS_EXECUTOR_CRASHED;

      appCloseExecutor(&executor);
      if (!appOpenExecutor(&executor)) {
        executorOpen = FALSE;
        break;
      }
    } else {
      entry.executorStatus = APP_DAEMON_TEST_RESPONSE_EXECUTOR_STATUS_NORMALLY_EXECUTED;
    }

    WriteFile(context->clientPipe, &entry, sizeof(entry), NULL, NULL);
  }

  entry.executorStatus = APP_DAEMON_TEST_RESPONSE_EXECUTOR_STATUS_EXECUTOR_CRASHED;
  while (ti < testSet.testCount)
    WriteFile(context->clientPipe, &entry, sizeof(entry), NULL, NULL);

  if (executorOpen) {
    const AppExecutorTaskType taskType = APP_EXECUTOR_TASK_TYPE_QUIT;
    WriteFile(executor.hStdin, &taskType, sizeof(taskType), NULL, NULL);
    appCloseExecutor(&executor);
  }
} // appDaemonContextHandleTest function end

//----------------------------------------------------------------
//! @brief SOLVE task handling function
//!
//! @param [in] context daemon context
//----------------------------------------------------------------
static void
appDaemonContextHandleSolve( AppDaemonContext *const context ) {
  AppDaemonSolveRequest req = {0};

  if (ReadFile(context->clientPipe, &req, sizeof(req), NULL, NULL)) {
    printf(
      "  SOLVE (%f %f %f)\n",
      req.coefficents.a,
      req.coefficents.b,
      req.coefficents.c
    );
  } else {
    printf("  SOLVE <invalid>\n");
    return;
  }

  AppDaemonSolveResponse res = {
    .status = APP_DAEMON_SOLVE_RESPONSE_STATUS_ERROR,
  };

  AppExecutor executor = {0};

  if (!appOpenExecutor(&executor)) {
    res.status = APP_DAEMON_SOLVE_RESPONSE_STATUS_ERROR;

    // send response to client
    WriteFile(context->clientPipe, &res, sizeof(res), NULL, NULL);
    return;
  }

  // read crash report
  AppExecutorCrashReport crashReport = {0};

  assert(executor.hStdin != NULL);
  assert(executor.hStdout != NULL);

  AppExecutorTaskType taskType;
  taskType = APP_EXECUTOR_TASK_TYPE_SOLVE;
  WriteFile(executor.hStdin, &taskType, sizeof(taskType), NULL, NULL);
  WriteFile(executor.hStdin, &req.coefficents, sizeof(req.coefficents), NULL, NULL);

  AppExecutorTaskStatus taskStatus = (AppExecutorTaskStatus)-1;
        
  if (!ReadFile(executor.hStdout, &taskStatus, sizeof(taskStatus), NULL, NULL)) {
    appCloseExecutor(&executor);
    WriteFile(context->clientPipe, &res, sizeof(res), NULL, NULL);
    return;
  }

  if (taskStatus != APP_EXECUTOR_TASK_STATUS_CRASHED) {
    if (!ReadFile(executor.hStdout, &res.solution, sizeof(res.solution), NULL, NULL)) {


      appCloseExecutor(&executor);
      WriteFile(context->clientPipe, &res, sizeof(res), NULL, NULL);
      return;
    }

    // send quit task
    taskType = APP_EXECUTOR_TASK_TYPE_QUIT;
    WriteFile(executor.hStdin, &taskType, sizeof(taskType), NULL, NULL);
    res.status = APP_DAEMON_SOLVE_RESPONSE_STATUS_OK;

  } else {
    if (ReadFile(executor.hStdout, &crashReport, sizeof(crashReport), NULL, NULL)) {
      printf("  EXECUTOR CRASHED WITH %d SIGNAL\n", crashReport.signal);
    } else {
      printf("  EXECUTOR CRASHED WITH <invalid> SIGNAL\n");
    }
  }

  appCloseExecutor(&executor);

  // send response to client
  WriteFile(context->clientPipe, &res, sizeof(res), NULL, NULL);
} // appDaemonContextHandleSolve function end


//----------------------------------------------------------------
//! @brief QUIT task handling function
//!
//! @param [in] context daemon context
//----------------------------------------------------------------
static void
appDaemonContextHandleQuit( AppDaemonContext *const context ) {
  printf("  SHUTDOWN\n");
  context->continueSession = FALSE;
} // appDaemonContextHandleQuit function end

//----------------------------------------------------------------
//! @brief daemon context initialization function
//!
//! @param [in] context daemon context
//! 
//! @return TRUE if successfully initialized, FALSE otherwise
//----------------------------------------------------------------
static BOOL
appDaemonContextInit( AppDaemonContext *const context ) {
  context->clientPipe = CreateNamedPipe(
    APP_DAEMON_CLIENT_PIPE,
    PIPE_ACCESS_DUPLEX,
    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
    PIPE_UNLIMITED_INSTANCES,
    512,
    512,
    5000,
    NULL
  );

  if (context->clientPipe == INVALID_HANDLE_VALUE) {
    printf("Failed to open input daemon pipe. ");
    appPrintWinapiError(stdout, GetLastError());
    printf("\n");
    return FALSE;
  }

  return TRUE;
} // appDaemonContextInit function end

//----------------------------------------------------------------
//! @brief daemon context deinitialization function
//!
//! @param [in] context daemon context
//----------------------------------------------------------------
static void
appDaemonContextClose( AppDaemonContext *const context ) {
  CloseHandle(context->clientPipe);
} // appDaemonContextClose function end

//----------------------------------------------------------------
//! @brief daemon context initialization function
//!
//! @param [in] context daemon context
//----------------------------------------------------------------
static void
appDaemonContextRun( AppDaemonContext *const context ) {

  while (TRUE) {
    printf("Waiting client... ");
    BOOL connected = ConnectNamedPipe(context->clientPipe, NULL);

    if (connected) {
      printf("Connected!\n");
    } else {
      printf("Error connecting to command pipe\n");
      continue;
    }

    BOOL continueSession = TRUE;

    while (continueSession) {
      AppDaemonRequestType requestType = APP_DAEMON_REQUEST_TYPE_QUIT;

      if (!ReadFile(context->clientPipe, &requestType, sizeof(requestType), NULL, NULL))
        break;

      switch (requestType) {
      case APP_DAEMON_REQUEST_TYPE_TEST :
        appDaemonContextHandleTest(context);
        break;

      case APP_DAEMON_REQUEST_TYPE_SOLVE    :
        appDaemonContextHandleSolve(context);
        break;

      case APP_DAEMON_REQUEST_TYPE_QUIT :
        appDaemonContextHandleQuit(context);
        break;

      default: {
        printf("  INVALID REQUEST TYPE: %d\n", requestType);
        continueSession = FALSE;
        break;
      }
      }
    }

    printf("Client disconnected.\n");
    DisconnectNamedPipe(context->clientPipe);
  }
} // daemonContextRun function end


int
appDaemonMain( int, const char ** ) {
  printf("DAEMON STARTED\n");

  AppDaemonContext daemonContext = {0};

  appDaemonContextInit(&daemonContext);
  appDaemonContextRun(&daemonContext);
  appDaemonContextClose(&daemonContext);

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
