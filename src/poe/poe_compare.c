#include "poe.h"

PoeOrdering POE_API
poeCompareInitialOrder( const PoeString *lhs, const PoeString *rhs ) {
  return poeCompareSize((size_t)lhs->first, (size_t)rhs->first);
} // poeCompareInitialOrder function end

/**
 * @brief cyrillic character test
 * 
 * @param c character to test
 * 
 * @return POE_TRUE if cyrillic in 1251, POE_FALSE otherwise
 */
static inline PoeBool
poeIsCyrillicAlpha( unsigned char c ) {
  return c >= 0xC0 && c <= 0xFF || c == 0xA8 || c == 0xB8;
} // poeIsCyrillicAlpha function end

PoeBool POE_API
poeCompareCheckCharacterComparability( unsigned char c ) {
  return poeIsCyrillicAlpha(c) || isdigit(c) || isalpha(c) || isspace(c);
} // poeCompareCheckCharacterComparability function end

unsigned char POE_API
poeCompareProcessCharacter( unsigned char c ) {
  // ASCII
  if (c <= 0x7F)
    return (unsigned char)tolower(c);

  // jo
  if (c == 0xA8 || c == 0xB8)
    return 0x86;

  // uppercase
  if (c <= 0xC5) return c - 0x40;
  if (c <= 0xDf) return c - 0x3F;

  // lowercase
  if (c <= 0xE5) return c - 0x60;
  if (c <= 0xFF) return c - 0x5F;

  // ERROR
  assert(POE_FALSE);
  return 0xFF;
} // poeCompareProcessCharacter function end

/**
 * @brief helper function
 * 
 * @param hs pointer to current character
 * 
 * @return helper structure
 */
static struct __PoeResultOf_poeCompareFromStartHelper {
  const char *  ptr; /// pointer
  unsigned char c;   /// character
}
poeCompareFromStartHelper( const char *const hs ) {
  struct __PoeResultOf_poeCompareFromStartHelper res = {
    .ptr = hs,
    .c = *hs,
  };

  while (res.c != '\0' && !poeCompareCheckCharacterComparability(res.c))
    res.c = *++res.ptr;
  res.c = poeCompareProcessCharacter(res.c);

  return res;
} // poeCompareFromStartHelper function end

PoeOrdering POE_API
poeCompareFromStart( const PoeString *const lhs, const PoeString *const rhs ) {
  assert(lhs != NULL);
  assert(rhs != NULL);

  struct __PoeResultOf_poeCompareFromStartHelper
    left  = { .ptr = lhs->first },
    right = { .ptr = rhs->first };

  while (POE_TRUE) {
    // function

    left  = poeCompareFromStartHelper(left.ptr);
    right = poeCompareFromStartHelper(right.ptr);

    if (left.c == '\0' || right.c == '\0' || left.c != right.c)
      break;

    left.ptr++;
    right.ptr++;
  }

  return poeCompareSize(
    (unsigned char)poeCompareProcessCharacter(*left.ptr),
    (unsigned char)poeCompareProcessCharacter(*right.ptr)
  );
} // poeCompareFromStart function end

/**
 * @brief helper function
 * 
 * @param hs pointer to current character
 * 
 * @return helper structure
 */
static struct __PoeResultOf_poeCompareFromEndHelper {
  const char *  ptr; /// pointer
  unsigned char c;   /// character
}
poeCompareFromEndHelper( const char *const hs ) {
  struct __PoeResultOf_poeCompareFromEndHelper res = {
    .ptr = hs,
    .c = *hs,
  };

  while (res.c != '\0' && !poeCompareCheckCharacterComparability(res.c))
    res.c = *--res.ptr;
  res.c = poeCompareProcessCharacter(res.c);

  return res;
} // poeCompareFromEndHelper function end

PoeOrdering POE_API
poeCompareFromEnd( const PoeString *const lhs, const PoeString *const rhs ) {
  assert(lhs != NULL);
  assert(rhs != NULL);

  struct __PoeResultOf_poeCompareFromEndHelper
    left  = { .ptr = lhs->last, },
    right = { .ptr = rhs->last, };

  while (POE_TRUE) {
    // function

    left  = poeCompareFromEndHelper(left.ptr);
    right = poeCompareFromEndHelper(right.ptr);

    if (left.c == '\0' || right.c == '\0' || left.c != right.c)
      break;

    left.ptr--;
    right.ptr--;
  }

  return poeCompareSize(
    (unsigned char)poeCompareProcessCharacter(*left.ptr),
    (unsigned char)poeCompareProcessCharacter(*right.ptr)
  );
} // poeCompareFromEnd function end

// poe_compare.c file end
