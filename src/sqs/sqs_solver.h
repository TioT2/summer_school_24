#ifndef SQS_SOLVER_H_
#define SQS_SOLVER_H_

#include "sqs_common.h"

/// @brief quadratic equation solve status enumeration
typedef enum __SqsQuadraticSolveStatus {
  /// No roots
  SQS_QUADRATIC_SOLVE_STATUS_NO_ROOTS,

  /// Single root
  SQS_QUADRATIC_SOLVE_STATUS_ONE_ROOT,

  /// Two roots
  SQS_QUADRATIC_SOLVE_STATUS_TWO_ROOTS,

  /// Root is any number
  SQS_QUADRATIC_SOLVE_STATUS_INF_ROOTS,
} SqsQuadraticSolveStatus;

/// @brief quadratic equation coefficents representation structure
typedef struct __SqsQuadraticEquationCoefficents {
  float a; /// a coefficent
  float b; /// b coefficent
  float c; /// c coefficent
} SqsQuadraticEquationCoefficents;

/// @brief quadratic equation solution representation structure
typedef struct __SqsQuadraticSolution {
  SqsQuadraticSolveStatus status; /// solve status
  float result1;                  /// first result
  float result2;                  /// second result
} SqsQuadraticSolution;

//----------------------------------------------------------------
//! @brief quadratic equation solve function
//!
//! @param [in]  coefs    coefficents of quadratic equation, requred to be valid
//! @param [out] solution solution structure pointer
//----------------------------------------------------------------
void SQS_API
sqsSolveQuadratic( const SqsQuadraticEquationCoefficents *const coefs, SqsQuadraticSolution *const solution );

//----------------------------------------------------------------
//! @brief quadratic equation ASCII serialization function
//!
//! @param [in] stream   stream to serialize solution to
//! @param [in] solution solution to serialize
//----------------------------------------------------------------
void SQS_API
sqsPrintQuadraticSolution( FILE *const stream, const SqsQuadraticSolution *const solution );

//----------------------------------------------------------------
//! @brief quadratic equation coefficents validation function
//!
//! @param [in] coef coefficents to validate
//!
//! @return SQS_TRUE if coefficents are valid, SQS_FALSE otherwise
//----------------------------------------------------------------
SqsBool SQS_API
sqsValidateQuadraticEquationCoefficents( const SqsQuadraticEquationCoefficents *const coef );

//----------------------------------------------------------------
//! @brief quadratic equation ASCII deserialization function
//!
//! @param [in]  stream stream to deserialize coefficents from
//! @param [out] dst    coeffecent holder pointer to parse in
//!
//! @return SQS_TRUE if coefficents parsed, SQS_FALSE otherwise
//----------------------------------------------------------------
SqsBool SQS_API
sqsParseQuadraticEquationCoefficents( FILE *const stream, SqsQuadraticEquationCoefficents *const dst );

//----------------------------------------------------------------
//! @brief quadratic equation solutions comparing function
//!
//! @param [in] lhs first solution
//! @param [in] rhs second solution
//!
//! @return SQS_TRUE if solutions are same, SQS_FALSE otherwise
//----------------------------------------------------------------
SqsBool SQS_API
sqsQuadraticSolutionEqual( const SqsQuadraticSolution *const lhs, const SqsQuadraticSolution *const rhs );

#endif // !defined(SQS_SOLVER_H_)