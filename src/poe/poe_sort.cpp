/**
 * @file   poe/poe_sort.cpp
 * @author tiot2
 * @brief  Poem processor text sort implementation module
 */

#include "poe.h"

/**
 * @brief QSORT partition function
 * 
 * @param array     text array
 * @param begin     begin
 * @param end       inclusive (!!!) end
 * @param compareFn compare function
 * 
 * @return partition index
 */
static size_t
poeTextQsortPartition( PoeString *const array, const size_t begin, const size_t end, const PoeStringCompareFn compareFn ) {
  const PoeString pivot = array[(begin + end) / 2];
  PoeString tmp;

  size_t i = begin;
  size_t j = end;

  while (POE_TRUE) {
    while (compareFn(array + i, &pivot) == POE_ORDERING_LESS)
      i++;

    while (compareFn(array + j, &pivot) == POE_ORDERING_MORE)
      j--;

    if (i >= j)
      return j;

    memcpy(&tmp     , array + i, sizeof(PoeString));
    memcpy(array + i, array + j, sizeof(PoeString));
    memcpy(array + j, &tmp     , sizeof(PoeString));

    i++;
    j--;
  }
} // poeTextQsortPartition function end

/**
 * @brief text sorting function
 * 
 * @param arr       array to sort
 * @param begin     begin index
 * @param end       inclusive (!!!) end index
 * @param compareFn compare function
 */
static void
poeTextQsort( PoeString *const arr, const size_t begin, const size_t end, const PoeStringCompareFn compareFn ) {
  if (begin < end) {
    const size_t partition = poeTextQsortPartition(arr, begin, end, compareFn);

    poeTextQsort(arr, begin, partition, compareFn);
    poeTextQsort(arr, partition + 1, end, compareFn);
  }
} // poeTextQsort function end

void POE_API
poeSortText( PoeText *const text, const PoeStringCompareFn compareFn ) {
  assert(text != NULL);
  assert(compareFn != NULL);

  if (text->stringCount < 2) {
    return;
  }

  poeTextQsort(text->strings, 0, text->stringCount - 1, compareFn);
} // poeSortText function end

int __cdecl
poeStdCompareWrapper( void *compareFn, const void *lhs, const void *rhs ) {
  return ((PoeStringCompareFn)compareFn)((const PoeString *)lhs, (const PoeString *)rhs);
} // poeStdCompareWrapper function end

void POE_API
poeSortTextStd( PoeText *const text, const PoeStringCompareFn compareFn ) {
  assert(text != NULL);
  assert(compareFn != NULL);

  qsort_s(text->strings, text->stringCount, sizeof(PoeString), poeStdCompareWrapper, (void *)compareFn);
} // poeSortTextStd function end

// poe_sort.c file end
