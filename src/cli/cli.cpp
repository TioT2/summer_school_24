#include <sqs/test/sqs_test.h>

#include "cli.h"

//----------------------------------------------------------------
//! @brief test performing function
//! 
//! @return SQS_TRUE if all tests Ok, SQS_FALSE otherwise.
//----------------------------------------------------------------
SqsBool
cliRunTests( void ) {
  SqsTestQuadraticFeedback feedback[5] = {0};
  SqsQuadraticTest tests[5] = {
    { { +0.0f, +0.0f, +0.0f, }, { SQS_QUADRATIC_SOLVE_STATUS_INF_ROOTS, +0.0000000000000000f, +0.0000000000000000f, } },
    { { +1.0f, +0.0f, +0.0f, }, { SQS_QUADRATIC_SOLVE_STATUS_ONE_ROOT,  +0.0000000000000000f, +0.0000000000000000f, } },
    { { +1.0f, +0.0f, +1.0f, }, { SQS_QUADRATIC_SOLVE_STATUS_NO_ROOTS,  +0.0000000000000000f, +0.0000000000000000f, } },
    { { +0.0f, +2.0f, +1.0f, }, { SQS_QUADRATIC_SOLVE_STATUS_ONE_ROOT,  -0.5000000000000000f, +0.0000000000000000f, } },
    { { +1.0f, -2.0f, -1.0f, }, { SQS_QUADRATIC_SOLVE_STATUS_TWO_ROOTS, -0.4142135623730951f, +2.4142135623730951f, } },
  };

  size_t testCount = sizeof(tests) / sizeof(tests[0]);

  SqsQuadraticTestSet testSet = {
    .tests = tests,
    .testCount = sizeof(tests) / sizeof(tests[0]),
  };

  // test set
  sqsTestQuadraticRunSet(sqsSolveQuadratic, &testSet, feedback);

  SqsBool result = SQS_TRUE;
  for (uint32_t i = 0; i < testCount; i++) {
    result &= feedback[i].ok;
    sqsPrintTestQuadraticFeedback(stdout, i + 1, feedback);
  }

  return result;
} // cliRunTests function end

//----------------------------------------------------------------
//! @brief stdin from console input clearing function
//----------------------------------------------------------------
void
cliClearStdin( void ) {
  while (getchar() != '\n')
    ;
} // cliClearStdin function end

//----------------------------------------------------------------
//! @brief menu running function
//----------------------------------------------------------------
void
cliRunMenu( void ) {
  printf("# Quadratic equation solver project\n");

  while (SQS_TRUE) {
    printf("# Enter quadratic equation coefficents (float a, float b, float c): ");
    SqsQuadraticEquationCoefficents coefs = {
      .a = 0.0f,
    };

    SqsBool cond = sqsParseQuadraticEquationCoefficents(stdin, &coefs);
    cliClearStdin();

    if (!cond) {
      printf("# Coefficent parsing error occured\n");
      continue;
    }
    
    SqsQuadraticSolution solution = {
      .status = SQS_QUADRATIC_SOLVE_STATUS_NO_ROOTS,
    };

    printf("Solution: ");
    sqsSolveQuadratic(&coefs, &solution);
    sqsPrintQuadraticSolution(stdout, &solution);
    putchar('\n');
  }
} // runMenu function end

//----------------------------------------------------------------
//! @brief help running function
//----------------------------------------------------------------
void
cliRunHelp( void ) {
  puts(
    "AVAILABLE KEYS: "
    "  --help   [default: false] Display command list\n"
    "  --nomenu [default: false] Disable menu\n"
#ifdef SQS_BUILD_CONFIGURATION_DEBUG
    "  --test   [default: false] Run tests\n"
#endif
  );
} // runHelp function end
