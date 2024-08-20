#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "sqs.h"

static SqsBool sqsFloatEqual( const float a, const float b ) {
  return fabsf(a - b) <= SQS_FLOAT_EPSILON;
} // sqsFloatEqual function end

void SQS_API sqsSolveQuadratic( const SqsQuadraticEquationCoefficents *const coefs, SqsQuadraticSolution *const solution ) {
  assert(solution != NULL);
  assert(coefs != NULL);
  assert(sqsValidateQuadraticEquationCoefficents(coefs));

  if (sqsFloatEqual(coefs->a, 0.0f)) {
    if (sqsFloatEqual(coefs->b, 0.0f)) {
      if (sqsFloatEqual(coefs->c, 0.0f)) {
        solution->status = SQS_QUADRATIC_SOLVE_STATUS_ANY_NUMBER;
        return;
      }
      solution->status = SQS_QUADRATIC_SOLVE_STATUS_NO_ROOTS;
      return;
    }

    solution->result1 = -coefs->c / coefs->b;
    solution->status = SQS_QUADRATIC_SOLVE_STATUS_ONE_ROOT;
    return;
  }

  const float discriminant = coefs->b * coefs->b - 4.0f * coefs->a * coefs->c;

  if (discriminant < 0.0f) {
    solution->status = SQS_QUADRATIC_SOLVE_STATUS_NO_ROOTS;
    return;
  }

  const float s = -coefs->b / (coefs->a * 2.0f);

  if (sqsFloatEqual(discriminant, 0.0f)) {
    solution->result1 = s;
    solution->status = SQS_QUADRATIC_SOLVE_STATUS_ONE_ROOT;
    return;
  }

  const float discriminant_sqr = sqrtf(discriminant) / (coefs->a * 2.0f);
  
  solution->result1 = s + discriminant_sqr;
  solution->result2 = s - discriminant_sqr;
  solution->status = SQS_QUADRATIC_SOLVE_STATUS_TWO_ROOTS;
} // sqsSolveQuadratic function end

void SQS_API sqsPrintQuadraticSolution( FILE *const stream, const SqsQuadraticSolution *const solution ) {
  assert(solution != NULL);
  assert(stream != NULL);

  switch (solution->status) {
    case SQS_QUADRATIC_SOLVE_STATUS_NO_ROOTS: {
      fprintf(stream, "No roots\n");
      break;
    }
    case SQS_QUADRATIC_SOLVE_STATUS_ONE_ROOT: {
      fprintf(stream, "Root: %f\n", solution->result1);
      break;
    }
    case SQS_QUADRATIC_SOLVE_STATUS_TWO_ROOTS: {
      fprintf(stream, "Roots: (%f, %f)\n", solution->result1, solution->result2);
      break;
    }
    case SQS_QUADRATIC_SOLVE_STATUS_ANY_NUMBER: {
      fprintf(stream, "Any number\n");
      break;
    }
  }
} // sqsPrintfQuadraticSolution function end

SqsBool SQS_API sqsValidateQuadraticEquationCoefficents( const SqsQuadraticEquationCoefficents *const coef ) {
  assert(coef != NULL);
  return isfinite(coef->a) && isfinite(coef->b) && isfinite(coef->c);
} // sqsValidateQuadraticEquationCoefficents function end

SqsBool SQS_API sqsParseQuadraticEquationCoefficents( FILE *const stream, SqsQuadraticEquationCoefficents *const dst ) {
  assert(stream != NULL);
  assert(dst != NULL);

  return
    fscanf_s(stream, "%f %f %f", &dst->a, &dst->b, &dst->c) == 3 &&
    sqsValidateQuadraticEquationCoefficents(dst) // idk, should I include validation in parsing function
  ;
} // sqsParseQuadraticEquationCoefficents function end
