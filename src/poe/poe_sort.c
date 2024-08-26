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
poeTextQsortPartition( const char **array, const size_t begin, const size_t end, const PoeStringCompareFn compareFn ) {
  const char *pivot = array[(begin + end) / 2];

  size_t i = begin;
  size_t j = end;

  while (POE_TRUE) {
    while (compareFn(array[i], pivot) == POE_ORDERING_LESS)
      i++;

    while (compareFn(array[j], pivot) == POE_ORDERING_MORE)
      j--;

    if (i >= j)
      return j;

    const char *const tmp = array[i];
    array[i] = array[j];
    array[j] = tmp;

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
poeTextQsort( const char **arr, size_t begin, size_t end, const PoeStringCompareFn compareFn ) {
  if (begin < end) {
    size_t partition = poeTextQsortPartition(arr, begin, end, compareFn);
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

/**
 * @brief standard library qsort_s compare function wrapper for POE compare functions
 * @param compareFn compare function (passed through qsort_s context)
 * @param lhs       left hand side
 * @param rhs       right hand side
 * @return compare result
 */
static int __cdecl
poeStdCompareWrapper( void *compareFn, const void *lhs, const void *rhs ) {
  return ((PoeStringCompareFn)compareFn)((const char *)lhs, (const char *)rhs);
} // poeStdCompareWrapper function end

void POE_API
poeSortTextStd( PoeText *const text, const PoeStringCompareFn compareFn ) {
  assert(text != NULL);
  assert(compareFn != NULL);

  qsort_s(text->strings, text->stringCount, sizeof(char *), poeStdCompareWrapper, (void *)compareFn);
} // poeSortTextStd function end

// poe_sort.c file end
