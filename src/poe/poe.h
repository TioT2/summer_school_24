#ifndef POE_H_
#define POE_H_

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>

/// Boolean type
typedef int PoeBool;

/// TRUE
#define POE_TRUE 1

/// FALSE
#define POE_FALSE 0

/// Calling convention
#define POE_API __cdecl

/// Ordering
typedef enum __PoeOrdering {
  /// Lhs less than rhs
  POE_ORDERING_LESS = -1,

  /// Lhs is equal to rhs
  POE_ORDERING_EQUAL = 0,

  /// Lhs is more than rhs
  POE_ORDERING_MORE = 1,
} PoeOrdering;

/**
 * @brief the most basic integer type comparing function
 * @param lhs left hand side
 * @param rhs right hand side
 * @return operand ordering
 */
inline PoeOrdering poeCompareSize( const size_t lhs, const size_t rhs ) {
  return lhs < rhs
    ? POE_ORDERING_LESS
    : lhs > rhs
      ? POE_ORDERING_MORE
      : POE_ORDERING_EQUAL;
} // poeCompareSize function end

typedef struct __PoeString {
  char *first; // first character pointer
  char *last;  // last character pointer
} PoeString;

/// Text representation structure
typedef struct __PoeText {
  char      * stringBuffer; ///< string bulk allocation
  PoeString * strings;      ///< text string pointer
  size_t      stringCount;  ///< count of text strings
} PoeText;

/**
 * @brief Text parsing function
 * 
 * @param file    file with text data
 * @param dstText parsed text
 * 
 * @return POE_TRUE if parsed successfully, POE_FALSE otherwise
 */
PoeBool POE_API
poeParseText( FILE *const file, PoeText *const dst );

/**
 * @brief Text writing function
 * 
 * @param file file to write text to
 * @param text text to write
 */
void POE_API
poeWriteText( FILE *const file, const PoeText *const text );

/**
 * @brief text destructor
 * 
 * @param text text to destroy
 */
void POE_API
poeDestroyText( PoeText *const text );

/**
 * @brief text shuffle function
 * 
 * @param text             text to shuffle
 * @param permutationCount count of permutations to perform
 */
void POE_API
poeShuffleText( PoeText *const text, size_t permutationCount );

/**
 * @brief string compare function pointer definition
 * 
 * @param lhs first string
 * @param rhs second string
 * 
 * @note SAFETY: caller MUST guarantee that lhs and rhs belong to same allocation
 * 
 * @return lhs and rhs element ordering
 */
typedef PoeOrdering
(POE_API * PoeStringCompareFn)( const PoeString *lhs, const PoeString *rhs );

/**
 * @brief text sorting function
 * @param text      text to sort
 * @param compareFn compare function
 */
void POE_API
poeSortText( PoeText *text, const PoeStringCompareFn compareFn );

/**
 * @brief text by standard qsort_s sorting function
 * @param text      text to sort
 * @param compareFn compare function
 */
void POE_API
poeSortTextStd( PoeText *text, const PoeStringCompareFn compareFn );

/***
 * compare functions (group PoeStringCompareFunctions)
 ***/

/**
 * @brief by initial parsing order comparing function
 * @ingroup PoeStringCompareFunctions
 * 
 * @param lhs left hand side
 * @param rhs right hand side
 * 
 * @return ordering
 */
PoeOrdering POE_API
poeCompareInitialOrder( const PoeString *lhs, const PoeString *rhs );

/**
 * @brief from begin to end comparing function
 * @ingroup PoeStringCompareFunctions
 * 
 * @param lhs left hand side
 * @param rhs right hand side
 * 
 * @return ordering
 */
PoeOrdering POE_API
poeCompareFromStart( const PoeString *lhs, const PoeString *rhs );

/**
 * @brief from end to begin comparing function
 * @ingroup PoeStringCompareFunctions
 * 
 * @param lhs left hand side
 * @param rhs right hand side
 * 
 * @return ordering
 */
PoeOrdering POE_API
poeCompareFromEnd( const PoeString *lhs, const PoeString *rhs );

#endif // !defined(POE_H_)

// poe.h file end
