#include <assert.h>
#include <string.h>

#include "__sqs_unit_test.h"

void
sqsPrintTestQuadraticFeedback(
  FILE *const file,
  int testIndex,
  const SqsTestQuadraticFeedback *const feedback
) {
  assert(file != NULL);
  assert(feedback != NULL);

  fprintf(file, "TEST %3d: ", testIndex);
  if (feedback->ok) {
    fprintf(file, CLI_SET_FOREGROUND_COLOR(0, 255, 0)"Ok"CLI_RESET_COLOR());
  } else {
    fprintf(file, CLI_SET_FOREGROUND_COLOR(255, 0, 0)"Error. Coefficents: (%f, %f, %f), Answer/Expected: "CLI_RESET_COLOR(),
      feedback->coefficents.a,
      feedback->coefficents.b,
      feedback->coefficents.c
    );

    sqsPrintQuadraticSolution(file, &feedback->actualSolution);
    fprintf(file, "/");
    sqsPrintQuadraticSolution(file, &feedback->expectedSolution);
  }

  fprintf(file, "\n");
} // sqsPrintTestQuadraticFeedback function end

void
sqsTestQuadraticRunTest(
  SqsTestQuadraticFeedback *const feedback,
  const SqsTestQuadraticSolver solver,
  const SqsQuadraticEquationCoefficents *const equation,
  const SqsQuadraticSolution *const expectedSolution
) {
  assert(feedback != NULL);
  assert(solver != NULL);
  assert(equation != NULL);
  assert(expectedSolution != NULL);

  memcpy(&feedback->expectedSolution, expectedSolution, sizeof(SqsQuadraticSolution));
  memcpy(&feedback->coefficents, equation, sizeof(SqsQuadraticEquationCoefficents));
  solver(equation, &feedback->actualSolution);
  feedback->ok = sqsQuadraticSolutionEqual(&feedback->actualSolution, &feedback->expectedSolution);
} // sqsTestQuadraticRunTest function end

void
sqsTestQuadraticRunStandardTests(
  const SqsTestQuadraticSolver solver,
  size_t *testFeedbackCount,
  SqsTestQuadraticFeedback *testFeedbacks
) {
  assert(solver != NULL);
  assert(testFeedbackCount != NULL);

  static const struct {
    SqsQuadraticEquationCoefficents coefficents;
    SqsQuadraticSolution expectedSolution;
  } tests[] = {
    { { +0.0f, +0.0f, +0.0f, }, { SQS_QUADRATIC_SOLVE_STATUS_ANY_NUMBER, +0.0000000000000000f, +0.0000000000000000f, } },
    { { +1.0f, +0.0f, +0.0f, }, { SQS_QUADRATIC_SOLVE_STATUS_ONE_ROOT,   +0.0000000000000000f, +0.0000000000000000f, } },
    { { +1.0f, +0.0f, +1.0f, }, { SQS_QUADRATIC_SOLVE_STATUS_NO_ROOTS,   +0.0000000000000000f, +0.0000000000000000f, } },
    { { +0.0f, +2.0f, +1.0f, }, { SQS_QUADRATIC_SOLVE_STATUS_ONE_ROOT,   -0.5000000000000000f, +0.0000000000000000f, } },
    { { +1.0f, -2.0f, -1.0f, }, { SQS_QUADRATIC_SOLVE_STATUS_TWO_ROOTS,  -0.4142135623730951f, +2.4142135623730951f, } },
  };
 
  static const size_t TEST_COUNT = sizeof(tests) / sizeof(tests[0]);

  *testFeedbackCount = TEST_COUNT;
  
  if (testFeedbacks == NULL)
    return;

  for (int i = 0; i < TEST_COUNT; i++)
    sqsTestQuadraticRunTest(
      testFeedbacks + i,
      solver,
      &tests[i].coefficents,
      &tests[i].expectedSolution
    );
} // sqsTestQuadraticRunStandardTests function end

// __sqs_unit_test.c file end
