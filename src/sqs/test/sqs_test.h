#ifndef __SQS_UNIT_TEST_H_
#define __SQS_UNIT_TEST_H_

#include "../sqs.h"
#include "cli/cli.h"

/// @brief Solver function type definition
typedef void (SQS_API *SqsTestQuadraticSolver)( const SqsQuadraticEquationCoefficents *, SqsQuadraticSolution * );

/// @brief quadratic equation solve test feedback representation structure
typedef struct __SqsTestQuadraticFeedback {
  SqsBool                         ok;               // test status; SQS_TRUE if succeeded
  SqsQuadraticEquationCoefficents coefficents;      // tested equation coefficents
  SqsQuadraticSolution            expectedSolution; // expected equation solution
  SqsQuadraticSolution            actualSolution;   // actual equation solution
} SqsTestQuadraticFeedback;

/// @brief quadratic test representation structure
typedef struct __SqsQuadraticTest {
  SqsQuadraticEquationCoefficents coefficents;      // test coefficents
  SqsQuadraticSolution            expectedSolution; // expected test solution
} SqsQuadraticTest;

/// @brief set of test representation structure
typedef struct __SqsQuadraticTestSet {
  SqsQuadraticTest * tests;     // test array
  uint32_t           testCount; // test array size
} SqsQuadraticTestSet;

//----------------------------------------------------------------
//! @brief test set from string parsing function
//!
//! @param [in]  source test set source
//! @param [out] set    set to write pointer
//! 
//! @return SQS_TRUE if parsed successfully, SQS_FALSE otherwise
//----------------------------------------------------------------
SqsBool
sqsParseQuadraticTestSet(
  const char *const source,
  SqsQuadraticTestSet *const set
);

//----------------------------------------------------------------
//! @brief test set destroying function
//!
//! @param [in] testSet test set to destroy
//----------------------------------------------------------------
void
sqsDestroyQuadraticTestSet( SqsQuadraticTestSet *const testSet );

//----------------------------------------------------------------
//! @brief test feedback printing function
//!
//! @param [in] file      file to print feedback in
//! @param [in] testIndex index of test
//! @param [in] feedback  feedback to print
//----------------------------------------------------------------
void
sqsPrintTestQuadraticFeedback(
  FILE *const file,
  int testIndex,
  const SqsTestQuadraticFeedback *const feedback
);

//----------------------------------------------------------------
//! @brief single test performing function
//!
//! @param [out] feedback feedback to write during test performing
//! @param [in]  solver   tested solver
//! @param [in]  test     test pointer
//----------------------------------------------------------------
void
sqsTestQuadraticRunTest(
  SqsTestQuadraticFeedback *const feedback,
  const SqsTestQuadraticSolver solver,
  const SqsQuadraticTest *const test
);

#endif // !defined(__SQS_UNIT_TEST_H_)

// __sqs_unit_test.h file end
