#include <assert.h>
#include <string.h>

#include "sqs_test.h"

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
    fprintf(
      file, (
        CLI_SET_FOREGROUND_COLOR(0, 255, 0)
        "Ok"
        CLI_RESET_COLOR()
      )
    );
  } else {
    fprintf(
      file, (
        CLI_SET_FOREGROUND_COLOR(255, 0, 0)
        "Error. Coefficents: (%f, %f, %f), Answer/Expected: "
        CLI_RESET_COLOR()
      ),
      feedback->coefficents.a,
      feedback->coefficents.b,
      feedback->coefficents.c
    );

    sqsPrintQuadraticSolution(file, &feedback->actualSolution);
    fprintf(file, "/");
    sqsPrintQuadraticSolution(file, &feedback->expectedSolution);
  }
} // sqsPrintTestQuadraticFeedback function end

void
sqsTestQuadraticRunTest(
  SqsTestQuadraticFeedback *const feedback,
  const SqsTestQuadraticSolver solver,
  const SqsQuadraticTest *const test
) {
  assert(feedback != NULL);
  assert(solver != NULL);
  assert(test != NULL);

  memcpy(&feedback->expectedSolution, &test->expectedSolution, sizeof(SqsQuadraticSolution));
  memcpy(&feedback->coefficents, &test->coefficents, sizeof(SqsQuadraticEquationCoefficents));
  solver(&test->coefficents, &feedback->actualSolution);
  feedback->ok = sqsQuadraticSolutionEqual(&feedback->actualSolution, &feedback->expectedSolution);
} // sqsTestQuadraticRunTest function end

// __sqs_unit_test.c file end
