#include "poe.h"

PoeOrdering POE_API
poeCompareInitialOrder( const char *lhs, const char *rhs ) {
  return poeCompareSize((size_t)lhs, (size_t)rhs);
} // poeCompareInitialOrder function end

PoeOrdering POE_API
poeCompareFromStart( const char *lhs, const char *rhs ) {
  assert(lhs != NULL);
  assert(rhs != NULL);

  while (POE_TRUE) {
    unsigned char lc = *lhs;
    while (lc != '\0' && !isdigit(lc) && !isalpha(lc) && !isspace(lc))
      lc = *++lhs;
    lc = (unsigned char)tolower(lc);

    unsigned char rc = *rhs;
    while (rc != '\0' && !isdigit(rc) && !isalpha(rc) && !isspace(rc))
      rc = *++rhs;
    rc = (unsigned char)tolower(rc);

    if (lc == '\0' || rc == '\0' || lc != rc)
      break;

    lhs++;
    rhs++;
  }

  return poeCompareSize((unsigned char)*lhs, (unsigned char)*rhs);
} // poeCompareFromStart function end

PoeOrdering POE_API
poeCompareFromEnd( const char *lhs, const char *rhs ) {
  assert(lhs != NULL);
  assert(rhs != NULL);

  lhs += strlen(lhs) - 1;
  rhs += strlen(rhs) - 1;

  while (POE_TRUE) {
    unsigned char lc = *lhs;
    while (lc != '\0' && !isdigit(lc) && !isalpha(lc) && !isspace(lc))
      lc = *--lhs;
    lc = (unsigned char)tolower(lc);

    unsigned char rc = *rhs;
    while (rc != '\0' && !isdigit(rc) && !isalpha(rc) && !isspace(rc))
      rc = *--rhs;
    rc = (unsigned char)tolower(rc);

    if (lc == '\0' || rc == '\0' || lc != rc)
      break;

    lhs--;
    rhs--;
  }

  // OK, because PoeText strings have 0 chraracter before and after it.
  return poeCompareSize((unsigned char)*lhs, (unsigned char)*rhs);
} // poeCompareFromEnd function end

// poe_compare.c file end
