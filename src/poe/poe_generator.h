#ifndef POE_GENERATOR_H_
#define POE_GENERATOR_H_

#include "poe_core.h"
#include "poe_compare.h"

/// line end representation structure
typedef struct __PoeEnding {
  uint32_t     last;        ///< last characters, preprocessed and compressed in string
  PoeString ** strings;     ///< list of indices of strings with this ending
  size_t       stringCount; ///< count of string endings
} PoeEnding;

/// poem generator representation structure
typedef struct __PoeGenerator {
  const PoeText   * text;        ///< text
  PoeString      ** stringPool;  ///< string pool
  PoeEnding       * endings;     ///< set of strings, qualified by ending
  size_t            endingCount; ///< count of endings
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
poeCreateGenerator(
  const PoeText *const text,
  PoeGenerator *const generator
);

/**
 * @brief string generator initialization function

 * @param generator generator to deinitialize
 */
void POE_API
poeDestroyGenerator(
  PoeGenerator *const generator
);

/**
 * @brief stanza generation function
 * 
 * @param generator generator to generate stanza in
 * 
 * @return generated stanza as text
 */
char * POE_API
poeGenerateOneginStanza( const PoeGenerator *const generator );

/**
 * @brief endings display function
 * 
 * @param file      ending file
 * @param generator ending generator
 */
void POE_API
poeGeneratorPrint(
  FILE *const file,
  const PoeGenerator *const generator
);

#endif // !defined(POE_GENERATOR_H_)

// poe_compare.h file end
