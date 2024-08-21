#include <string.h>

#include "sqs_test.h"

SqsBool
sqsParseQuadraticTestSet(
  const char *const source,
  SqsQuadraticTestSet *const set
) {
  assert(source != NULL);
  assert(set != NULL);
  
  size_t sourceLen = strlen(source);

  const char *sourceCurr = source;
  const char *const sourceEnd = source + sourceLen;

  SqsBool isOk = SQS_TRUE;

  size_t capacity = 2;
  size_t size = 0;

  set->tests = (SqsQuadraticTest *)malloc(sizeof(SqsQuadraticTest) * capacity);

  if (set->tests == NULL) {
    return SQS_FALSE;
  }

  while (sourceCurr < sourceEnd) {
    const char *const lineStart = sourceCurr;
    const char *lineEnd = NULL;

    while (sourceCurr < sourceEnd) {
      if (*sourceCurr == '\n') {
        lineEnd = sourceCurr;
        break;
      }
      sourceCurr++;
    }


    if (lineEnd == NULL) {
      lineEnd = sourceEnd;
    } else {
      sourceCurr += 1;
    }

    const char *commentStart = lineStart;

    while (commentStart < lineEnd && *commentStart != '#') {
      commentStart++;
    }

    lineEnd = commentStart;

    SqsQuadraticTest test = {0};
    char type[4] = {0};

    uint32_t count = (uint32_t)sscanf_s(
      lineStart,
      "%f %f %f %3s %f %f",
      &test.coefficents.a,
      &test.coefficents.b,
      &test.coefficents.c,
      type,
      4,
      &test.expectedSolution.result1,
      &test.expectedSolution.result2
    );

    if (count < 4)
      continue;

    static const struct {
      const char *name;
      uint32_t parameterCount;
      SqsQuadraticSolveStatus status;
    } stat[] = {
      {"ZER", 0, SQS_QUADRATIC_SOLVE_STATUS_NO_ROOTS },
      {"ONE", 1, SQS_QUADRATIC_SOLVE_STATUS_ONE_ROOT },
      {"TWO", 2, SQS_QUADRATIC_SOLVE_STATUS_TWO_ROOTS},
      {"INF", 0, SQS_QUADRATIC_SOLVE_STATUS_INF_ROOTS},
    };

    uint32_t expectedParameterCount = UINT32_MAX;
    for (size_t i = 0, n = sizeof(stat) / sizeof(stat[0]); i < n; i++) {
      if (strcmp(type, stat[i].name) == 0) {
        expectedParameterCount = stat[i].parameterCount;
        test.expectedSolution.status = stat[i].status;
        break;
      }
    }

    if (expectedParameterCount == UINT32_MAX) {
      isOk = SQS_FALSE;
      break;
    }

    if (count < expectedParameterCount + 4) {
      isOk = SQS_FALSE;
      break;
    }

    if (capacity - size < 1) {
      capacity *= 2;

      void *tests = set->tests;
      set->tests = (SqsQuadraticTest *)realloc(set->tests, sizeof(SqsQuadraticTest) * capacity);


      if (set->tests == NULL) {
        free(tests);
        return SQS_FALSE;
      }
    }

    memcpy(set->tests + size, &test, sizeof(SqsQuadraticTest));
    size++;
  }

  if (size != capacity) {
    void *tests = set->tests;
    set->tests = (SqsQuadraticTest *)realloc(set->tests, sizeof(SqsQuadraticTest) * size);
    if (set->tests == NULL) {
      free(tests);
      return SQS_FALSE;
    }
  }

  set->testCount = size;

  return SQS_TRUE;
} // sqsParseQuadraticTestSet function end

void
sqsDestroyQuadraticTestSet( SqsQuadraticTestSet *const testSet ) {
  free(testSet->tests);
} // sqsDestroyQuadraticTestSet function end

// sqs_test_set.cpp file end