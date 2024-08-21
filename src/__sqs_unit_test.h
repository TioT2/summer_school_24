#ifndef __SQS_UNIT_TEST_H_
#define __SQS_UNIT_TEST_H_

#include "sqs.h"

/// @brief Solver function type definition
typedef void (SQS_API *SqsTestQuadraticSolver)( const SqsQuadraticEquationCoefficents *, SqsQuadraticSolution * );

/// @brief quadratic equation solve test feedback representation structure
typedef struct __SqsTestQuadraticFeedback {
  SqsBool                         ok;               // test status; SQS_TRUE if succeeded
  SqsQuadraticEquationCoefficents coefficents;      // tested equation coefficents
  SqsQuadraticSolution            expectedSolution; // expected equation solution
  SqsQuadraticSolution            actualSolution;   // actual equation solution
} SqsTestQuadraticFeedback;

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
//! @param [out] feedback         feedback to write during test performing
//! @param [in]  solver           tested solver
//! @param [in]  equation         equation coefficents
//! @param [in]  expectedSolution expected equation solution
//----------------------------------------------------------------
void
sqsTestQuadraticRunTest(
  SqsTestQuadraticFeedback *const feedback,
  const SqsTestQuadraticSolver solver,
  const SqsQuadraticEquationCoefficents *const equation,
  const SqsQuadraticSolution *const expectedSolution
);

//----------------------------------------------------------------
//! @brief standard test set performing function
//!
//! @param [in]  solver            tested solver
//! @param [out] testFeedbacksSize required minimal size of test feedback array
//! @param [out] testFeedbacks     tests feedback array
//! 
//! @note testFeedbacks array may be null during first function call, \
//!       in this case only testFeedbacksSize value will be filled,   \
//!       testFeedbacksSize will be filled with same value during     \
//!       every function invocation
//----------------------------------------------------------------
void
sqsTestQuadraticRunStandardTests(
  const SqsTestQuadraticSolver solver,
  size_t *testFeedbacksSize,
  SqsTestQuadraticFeedback *testFeedbacks
);

#endif // !defined(__SQS_UNIT_TEST_H_)

// __sqs_unit_test.h file end
