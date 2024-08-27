/**
 * @file   poe/poe_compare.h
 * @author tiot2
 * @brief  Poem processor comparator declaration module
 */

#ifndef POE_COMPARE_H_
#define POE_COMPARE_H_

#include "poe_core.h"

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
 * 
 * @param lhs left hand side
 * @param rhs right hand side
 * 
 * @return ordering
 */
inline PoeOrdering POE_API
poeCompareSize( const size_t lhs, const size_t rhs ) {
  return lhs < rhs
    ? POE_ORDERING_LESS
    : lhs > rhs
      ? POE_ORDERING_MORE
      : POE_ORDERING_EQUAL;
} // poeCompareSize function end

/***
 * character utilities (group PoeCharacterUtilFunctions)
 ***/

/**
 * @brief check if character can be used
 * @ingroup PoeCharacterUtilFunctions
 * 
 * @param c character
 * 
 * @return POE_TRUE if ok, POE_FALSE if should be skipped
 */
PoeBool POE_API
poeCompareCheckCharacterComparability( unsigned char c );

/**
 * @brief character re-encoding function
 * @ingroup PoeCharacterUtilFunctions
 * 
 * @param c character
 * 
 * @note accepts ASCII or passed by poeCompareCheckCharacterComparability function characters only
 * 
 * @return re-encoded character
 */
unsigned char POE_API
poeCompareProcessCharacter( unsigned char c );

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

#endif // !defined(POE_COPMARE_H_)

// poe_compare.h file end
