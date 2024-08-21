#include <stdio.h>
#include <stdlib.h>

#include "sqs.h"
#include "__sqs_unit_test.h"

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

int main( void ) {
  runTests();

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

  return 0;
} // main function end

// sqs_main.c file end