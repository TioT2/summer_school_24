#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

// #include <signal.h>

#include "cli/cli.h"
#include "sqs/test/sqs_test.h"

// void signalHandler( int signal ) {
//   static const struct {
//     const char *name;
//     int signal;
//   } signalNameTable[] = {
//     {"SIGINT"  , SIGINT  },
//     {"SIGILL"  , SIGILL  },
//     {"SIGFPE"  , SIGFPE  },
//     {"SIGSEGV" , SIGSEGV },
//     {"SIGTERM" , SIGTERM },
//     {"SIGBREAK", SIGBREAK},
//     {"SIGABRT" , SIGABRT },
//   };
// 
//   const char *signalName = "UNKNOWN";
// 
//   for (uint32_t i = 0, n = sizeof(signalNameTable) / sizeof(signalNameTable[0]); i < n; i++) {
//     if (signalNameTable[i].signal == signal) {
//       signalName = signalNameTable[i].name;
//       break;
//     }
//   }
// 
//   printf(
//     CLI_SET_FOREGROUND_COLOR(255, 0, 0)
//     "!!SIGNAL: %s\n"
//     CLI_RESET_COLOR(),
//     signalName
//   );
//   exit(0);
// }

//----------------------------------------------------------------
//! @brief project entry point
//!
//! @param [in] argc count of console parameters
//! @param [in] argv parameter strings
//! 
//! @return exit status
//----------------------------------------------------------------
int
main(
  int argc,
  const char **argv
) {


  struct {
    SqsBool enableMenu;
    SqsBool enableHelp;
  } config = {
    .enableMenu  = SQS_TRUE,
    .enableHelp  = SQS_FALSE,
  };

  CliParameterIterator parameterContext;

  // Initialize parameter 'iterator'
  cliInitParameterIterator(argc, argv, &parameterContext);

  // Skip file path
  cliParameterIteratorNext(&parameterContext);

  const char *arg = NULL;

  while ((arg = cliParameterIteratorNext(&parameterContext)) != NULL) {

    if (strcmp(arg, "--nomenu") == 0) {
      config.enableMenu = SQS_FALSE;
      continue;
    }

    if (strcmp(arg, "--help") == 0) {
      config.enableHelp = SQS_TRUE;
      continue;
    }

    if (strcmp(arg, "--custom") == 0) {
      SqsQuadraticEquationCoefficents coefs = { NAN, NAN, NAN };

      struct {
        const char *pStr;
        float *p;
      } cpc[] = {
        {cliParameterIteratorNext(&parameterContext), &coefs.a},
        {cliParameterIteratorNext(&parameterContext), &coefs.b},
        {cliParameterIteratorNext(&parameterContext), &coefs.c},
      };

      // parse parameters
      for (int i = 0; i < 3; i++) {
        if (cpc[i].pStr == NULL) {
          printf(
            CLI_SET_FOREGROUND_COLOR(255, 0, 0)
            "NO ENOUGH PARAMETERS FOR CLI QUADRATIC EQUATION\n"
            CLI_RESET_COLOR()
          );
          return 0;
        }

        if (sscanf_s(cpc[i].pStr, "%f", cpc[i].p) != 1) {
          printf(
            CLI_SET_FOREGROUND_COLOR(255, 0, 0)
            "INVALID FLOATING POINT PARAMETER: %s\n"
            CLI_RESET_COLOR(),
            cpc[i].pStr
          );
        }
      }

      if (!sqsValidateQuadraticEquationCoefficents(&coefs)) {
        printf(
          CLI_SET_FOREGROUND_COLOR(255, 0, 0)
          "INVALID COEFFICENTS\n"
          CLI_RESET_COLOR()
        );
        return 0;
      }

      SqsQuadraticSolution solution = { SQS_QUADRATIC_SOLVE_STATUS_NO_ROOTS };

      sqsSolveQuadratic(&coefs, &solution);

      printf("Solution: ");
      sqsPrintQuadraticSolution(stdout, &solution);
      putchar('\n');

      continue;
    }

    printf(
      CLI_SET_FOREGROUND_COLOR(255, 0, 0)
      "UNKNOWN KEY \"%s\"\n"
      CLI_RESET_COLOR(),
      arg
    );
    return 0;
  }

  // signal(SIGINT  , signalHandler);
  // signal(SIGILL  , signalHandler);
  // signal(SIGFPE  , signalHandler);
  // signal(SIGSEGV , signalHandler);
  // signal(SIGTERM , signalHandler);
  // signal(SIGBREAK, signalHandler);
  // signal(SIGABRT , signalHandler);

  if (config.enableHelp)
    cliRunHelp();

  if (config.enableMenu)
    cliRunMenu();

  return 0;
} // main function end

// sqs_main.c file end
