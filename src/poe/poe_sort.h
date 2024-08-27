/**
 * @file   poe/poe_sort.h
 * @author tiot2
 * @brief  Poem processor text sort declaration module
 */

#ifndef POE_SORT_H_
#define POE_SORT_H_

#include "poe_core.h"
#include "poe_compare.h"

/**
 * @brief text sorting function
 * 
 * @param text      text to sort
 * @param compareFn compare function
 */
void POE_API
poeSortText( PoeText *text, const PoeStringCompareFn compareFn );

/**
 * @brief text by standard qsort_s sorting function
 * 
 * @param text      text to sort
 * @param compareFn compare function
 */
void POE_API
poeSortTextStd( PoeText *text, const PoeStringCompareFn compareFn );

/**
 * @brief text shuffle function
 * 
 * @param text             text to shuffle
 * @param permutationCount count of permutations to perform
 */
void POE_API
poeShuffleText( PoeText *const text, size_t permutationCount );

#endif // !defined(POE_SORT_H_)

// poe_sort.h file end
