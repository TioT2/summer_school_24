#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "cli/cli.h"

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
    SqsBool enableTests;
    SqsBool enableMenu;
    SqsBool enableHelp;
  } config = {
    .enableTests = SQS_FALSE,
    .enableMenu = SQS_TRUE,
    .enableHelp = SQS_FALSE,
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

    // debug-only keys handling
#ifdef SQS_BUILD_CONFIGURATION_DEBUG
    if (strcmp(arg, "--test") == 0) {
      config.enableTests = SQS_TRUE;
      continue;
    }
#endif

    printf(
      CLI_SET_FOREGROUND_COLOR(255, 0, 0)
      "UNKNOWN KEY \"%s\"\n"
      CLI_RESET_COLOR(),
      arg
    );
    return 0;
  }

  if (config.enableHelp)
    cliRunHelp();

  if (config.enableTests)
    cliRunTests();

  if (config.enableMenu)
    cliRunMenu();

  return 0;
} // main function end

// sqs_main.c file end
