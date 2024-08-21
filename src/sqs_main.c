#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sqs.h"
#include "__sqs_unit_test.h"

//----------------------------------------------------------------
//! @brief test performing function
//! 
//! @return SQS_TRUE if all tests Ok, SQS_FALSE otherwise.
//----------------------------------------------------------------
SqsBool runTests( void ) {

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
} // runTests function end

//----------------------------------------------------------------
//! @brief menu running function
//----------------------------------------------------------------
void
runMenu( void ) {
  printf("# Quadratic equation solver project\n");

  while (SQS_TRUE) {
    printf("# Enter quadratic equation coefficents (float a, float b, float c): ");
    SqsQuadraticEquationCoefficents coefs = {
      .a = 0.0f,
    };

    if (!sqsParseQuadraticEquationCoefficents(stdin, &coefs)) {
      printf("# Coefficent parsing error occured\n");
      while (getchar() != '\n') // function
        ;
      continue;
    }
    
    SqsQuadraticSolution solution = {
      .status = SQS_QUADRATIC_SOLVE_STATUS_NO_ROOTS,
    };

    sqsSolveQuadratic(&coefs, &solution);
    sqsPrintQuadraticSolution(stdout, &solution);
  }
} // runMenu function end

//----------------------------------------------------------------
//! @brief help running function
//----------------------------------------------------------------
void
runHelp( void ) {
  puts(
    "AVAILABLE KEYS: "
    "  -help   [default: false] Display command list\n"
    "  -nomenu [default: false] Disable menu\n"
#ifdef SQS_BUILD_CONFIGURATION_DEBUG
    "  -test   [default: false] Run tests\n"
#endif
  );
} // runHelp function end

//----------------------------------------------------------------
//! @brief quadratic equation solve function
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
  SqsBool enableTests = SQS_FALSE;
  SqsBool enableMenu = SQS_TRUE;
  SqsBool enableHelp = SQS_FALSE;

  for (int i = 1, n = argc; i < n; i++) {

    if (strcmp(argv[i], "-nomenu") == 0) {
      enableMenu = SQS_FALSE;
      continue;
    }

    if (strcmp(argv[i], "-help") == 0) {
      enableHelp = SQS_TRUE;
      continue;
    }

    // debug-only keys handling
#ifdef SQS_BUILD_CONFIGURATION_DEBUG
    if (strcmp(argv[i], "-test") == 0) {
      enableTests = SQS_TRUE;
      continue;
    }
#endif

    printf("UNKNOWN KEY \"%s\"\n", argv[i]);
    return 0;
  }

  if (enableHelp)
    runHelp();

  if (enableTests)
    runTests();

  if (enableMenu)
    runMenu();

  return 0;
} // main function end

// sqs_main.c file end
