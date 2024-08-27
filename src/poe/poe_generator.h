#ifndef POE_GENERATOR_H_
#define POE_GENERATOR_H_

#include "poe_core.h"
#include "poe_compare.h"

/// line end representation structure
typedef struct __PoeEnding {
  PoeString * stringEnds;     ///< list of strings with this very ending
  size_t      stringEndCount; ///< count of string ends
  char        last[3];        ///< last characters
} PoeEnding;

/// poem generator representation structure
typedef struct __PoeGenerator {
  const PoeText   * text;         ///< text
  PoeString       * stringBuffer; ///< pool of allocated strings
  PoeEnding       * endingPool;   ///< set of strings, qualified by ending
} PoeGenerator;

/**
 * @brief generator constructor
 * 
 * @param text      text to generate poems based on
 * @param generator generator, actually
 * 
 * @return POE_TRUE if initialization succeeded, POE_FALSE otherwise
 */
PoeBool POE_API
poeGeneartorCreate(
  const PoeText *const text,
  PoeGenerator *const generator
);

/**
 * @brief string generator initialization function

 * @param generator generator to deinitialize
 */
void POE_API
poeGeneratorDestroy(
  PoeGenerator *const generator
);

#endif // !defined(POE_GENERATOR_H_)

// poe_compare.h file end
