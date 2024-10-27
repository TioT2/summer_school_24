#include "app_executor_impl.h"

int
appExecutorMain( int, const char ** ) {
  HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

  appExecutorSetupSignals(hStdin, hStdout);

  AppExecutorTaskType taskType = APP_EXECUTOR_TASK_TYPE_QUIT;
  const AppExecutorTaskStatus okStatus = APP_EXECUTOR_TASK_STATUS_OK;
  BOOL continueExecution = TRUE;

  while (continueExecution && ReadFile(hStdin, &taskType, sizeof(taskType), NULL, NULL)) {
    switch (taskType) {
      case APP_EXECUTOR_TASK_TYPE_SOLVE : {
        SqsQuadraticEquationCoefficents coefficents = {0};
        SqsQuadraticSolution solution = {
          .status = SQS_QUADRATIC_SOLVE_STATUS_NO_ROOTS,
        };

        if (!ReadFile(hStdin, &coefficents, sizeof(coefficents), NULL, NULL)) {
          abort();
        }
        sqsSolveQuadratic(&coefficents, &solution);

        WriteFile(hStdout, &okStatus, sizeof(okStatus), NULL, NULL);
        WriteFile(hStdout, &solution, sizeof(solution), NULL, NULL);

        break;
      }

      case APP_EXECUTOR_TASK_TYPE_TEST  : {
        SqsQuadraticTest test;
        SqsTestQuadraticFeedback feedback;

        if (!ReadFile(hStdin, &test, sizeof(test), NULL, NULL)) {
          abort();
        }

        if (test.expectedSolution.status == SQS_QUADRATIC_SOLVE_STATUS_NO_ROOTS) {
          // raise SIGSEGV
          *(uint8_t *)NULL = 42;
        }

        sqsTestQuadraticRunTest(&feedback, sqsSolveQuadratic, &test);
        WriteFile(hStdout, &okStatus, sizeof(okStatus), NULL, NULL);
        WriteFile(hStdout, &feedback, sizeof(feedback), NULL, NULL);
        break;
      }

      case APP_EXECUTOR_TASK_TYPE_QUIT  :
        continueExecution = FALSE;
        break;
    }
  }
  return 0;
} // appExecutorMain function end
