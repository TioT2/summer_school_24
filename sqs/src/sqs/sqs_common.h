#ifndef SQS_COMMON_H_
#define SQS_COMMON_H_

#include <stdio.h>

/// @brief boolean type declaration
typedef int SqsBool;

/// @brief true
#define SQS_TRUE  1

/// @brief false
#define SQS_FALSE 0

/// @brief floating point number epsilon
#define SQS_FLOAT_EPSILON 0.000001f

/// @brief API)) calling convention
#define SQS_API __cdecl

// Set configuration flag
#ifdef _DEBUG
  #define SQS_BUILD_CONFIGURATION_DEBUG
#else
  #define SQS_BUILD_CONFIGURATION_RELEASE
#endif

#ifdef __cplusplus
  #ifdef _MSC_VER
    #define SQR_RESTRICT __restrict
  #else
    #define SQR_RESTRICT
  #endif
#else
  #define SQR_RESTRICT restrict
#endif

#endif // !defined(SQS_COMMON_H_)