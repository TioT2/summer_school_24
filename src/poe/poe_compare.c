#include "poe.h"

PoeOrdering POE_API
poeCompareInitialOrder( const char *lhs, const char *rhs ) {
  return poeCompareSize((size_t)lhs, (size_t)rhs);
} // poeCompareInitialOrder function end

PoeOrdering POE_API
poeCompareFromStart( const char *lhs, const char *rhs ) {
  assert(POE_FALSE);
  return POE_ORDERING_EQUAL;
} // poeCompareFromStart function end

PoeOrdering POE_API
poeCompareFromEnd( const char *lhs, const char *rhs ) {
  assert(POE_FALSE);
  return POE_ORDERING_EQUAL;
} // poeCompareFromEnd function end

// poe_compare.c file end
