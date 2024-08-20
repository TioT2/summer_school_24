#include <stdio.h>
#include <conio.h>

#include "sqs.h"

int main( void ) {
  printf("# Quadratic equation solver project\n");

  while (SQS_TRUE) {
    printf("# Enter quadratic equation coefficents (float a, float b, float c): ");
    SqsQuadraticEquationCoefficents coefs = {
      .a = 0.0f,
    };

    if (!sqsParseQuadraticEquationCoefficents(stdin, &coefs)) {
      printf("# Coefficent parsing error occured\n");
      while (getchar() != '\n')
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