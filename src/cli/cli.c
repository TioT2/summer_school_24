#include <sqs/__sqs_unit_test.h>

#include "cli.h"

//----------------------------------------------------------------
//! @brief test performing function
//! 
//! @return SQS_TRUE if all tests Ok, SQS_FALSE otherwise.
//----------------------------------------------------------------
SqsBool
cliRunTests( void ) {

  size_t testCount = 0;
  sqsTestQuadraticRunStandardTests(sqsSolveQuadratic, &testCount, NULL);

  SqsTestQuadraticFeedback *feedbacks = NULL;
  if ((feedbacks = malloc(sizeof(SqsTestQuadraticFeedback) * testCount)) == NULL)
    return SQS_FALSE;

  sqsTestQuadraticRunStandardTests(sqsSolveQuadratic, &testCount, feedbacks);

  SqsBool result = SQS_TRUE;
  for (size_t i = 0; i < testCount; i++) {
    result &= feedbacks[i].ok;
    sqsPrintTestQuadraticFeedback(stdout, (int)i + 1, feedbacks + i);
  }

  free(feedbacks);

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
